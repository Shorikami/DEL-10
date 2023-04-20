#include <dlpch.h>
#include "../Application.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

float RandomNum(float min, float max)
{
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

void InitImGUI()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
    ImGui_ImplOpenGL3_Init("#version 450");
}

void InitGLFW()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_Window = glfwCreateWindow(ScreenSize.x, ScreenSize.y, "DEL-10", NULL, NULL);
    if (m_Window == NULL)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(m_Window);
    glfwSetFramebufferSizeCallback(m_Window, framebuffer_size_callback);
    glfwSetCursorPosCallback(m_Window, mouse_callback);
    glfwSetScrollCallback(m_Window, scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        throw std::runtime_error("Failed to initialize GLAD");
    }

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);

    InitImGUI();
}

void InitFBOs()
{
    glGenFramebuffers(1, &depthFBO);

    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, ScreenSize.x, ScreenSize.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    GLfloat border[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);

    glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // ---------------------------------
    
    glGenFramebuffers(1, &sceneFBO);


    glGenTextures(1, &sceneTexture);
    glBindTexture(GL_TEXTURE_2D, sceneTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, ScreenSize.x, ScreenSize.y, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    // ---------------------------------

    glGenRenderbuffers(1, &depthRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, ScreenSize.x, ScreenSize.y);

    glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneTexture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void InitLights()
{
    for (unsigned i = 0; i < MAX_LIGHTS; ++i)
    {
        float xPos = RandomNum(-150.0f, 150.0f);
        float yPos = RandomNum(-10.0f, 150.0f);
        float zPos = RandomNum(-80.0f, 80.0f);
        float range = RandomNum(globalRadius, globalRadius);

        float r = RandomNum(0.0f, 1.0f);
        float g = RandomNum(0.0f, 1.0f);
        float b = RandomNum(0.0f, 1.0f);

        m_LightBuffer->GetData().position[i] = glm::vec4(xPos, yPos, zPos, 1.0f);
        m_LightBuffer->GetData().colorRadius[i] = glm::vec4(r, g, b, range);
    }
    m_LightBuffer->SetData();
}

void InitScene() 
{
    computeWorkX = (ScreenSize.x + (ScreenSize.x % 32)) / 32;
    computeWorkY = (ScreenSize.y + (ScreenSize.y % 32)) / 32;
    size_t tiles = computeWorkX * computeWorkY;
    size_t size = tiles * sizeof(DEL10::VisibilityIdx) * 10000;

    m_LightBuffer = new DEL10::UniformBuffer<DEL10::Light>(0);

    glGenBuffers(1, &m_Visibility);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_Visibility);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, 0, GL_STATIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    float quadVertices[] = 
    {
        // positions        // texture Coords
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };

    // setup plane VAO
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    InitFBOs();

    InitLights();
}

void DrawQuad()
{
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void Update()
{
    DEL10::Light& lightBuffer = m_LightBuffer->GetData();
    float min = -80.0f;
    float max = 80.0f;

    for (unsigned i = 0; i < numLights; ++i)
    {
        lightBuffer.colorRadius[i].w = globalRadius;
        lightBuffer.position[i].z = lightBuffer.position[i].z + sinf(angleOfRotation) * 0.1f;
    }

    m_LightBuffer->SetData();

    angleOfRotation += (2.0f * glm::pi<float>() / 180.0f) / 10.0f;
}

void UpdateImGUI()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    float frames = 1.0f / deltaTime;

    ImGui::Begin("Tools");
    ImGui::Text("FPS: %.2f", frames);
    ImGui::Separator();

    if (ImGui::Button("Reload Shaders"))
        ReloadShaders();

    ImGui::SameLine(); ImGui::Text("Camera Position: (%.2f, %.2f, %.2f)", 
        m_Camera.cameraPos.x, m_Camera.cameraPos.y, m_Camera.cameraPos.z);

    ImGui::SliderInt("Light Count", &numLights, 1, 10000);
    ImGui::SliderFloat("Light Radius (Global)", &globalRadius, 0.1f, 50.0f);
    ImGui::SliderFloat("Camera Speed", &m_Camera.speed, 0.1f, 25.0f);

    ImGui::Checkbox("Display Light Positions", &displayLights);
    if (displayLights)
        ImGui::Checkbox("Display Light Ranges", &displayLightRanges);
    ImGui::Checkbox("Move Lights", &moveLights);
    ImGui::Checkbox("Show Light Overlap", &debugLight);

    if (debugLight)
    {
        ImGui::Text("Legend:");
        ImGui::Text("Red: No Lights");
        ImGui::Text("Green: 0 - 50 Lights");
        ImGui::Text("Blue: 50 - 100 Lights");
        ImGui::Text("Purple: 100 - 1000 Lights");
        ImGui::Text("White: ?");
    }

    ImGui::Text("Displaying...");
    const char* items[] = { "Scene", "Depth" };
    static const char* select = "Scene";
    if (ImGui::BeginCombo("##combo", select))
    {
        for (unsigned n = 0; n < IM_ARRAYSIZE(items); ++n)
        {
            bool is_selected = (select == items[n]);
            if (ImGui::Selectable(items[n], is_selected))
            {
                select = items[n];
                displayMode = n;
            }
            if (is_selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::Separator();
    ImGui::Checkbox("Use Forward Rendering", &useForward);

    ImGui::End();

    ImGui::Render();
}

void ReloadShaders()
{
    delete m_DepthPass;
    delete m_LightCulling;
    delete m_Lighting;
    delete m_LightingForward;

    m_DepthPass = new Shader("Assets/Shaders/DepthPrepass.vert", "Assets/Shaders/DepthPrepass.frag");
    m_Lighting = new Shader("Assets/Shaders/Lighting.vert", "Assets/Shaders/Lighting.frag");
    m_LightingForward = new Shader("Assets/Shaders/Lighting.vert", "Assets/Shaders/LightingNoCheck.frag");
    m_LightCulling = new Shader("Assets/Shaders/LightCulling.cmpt");
}

int main()
{
    InitGLFW();

    m_DepthPass = new Shader("Assets/Shaders/DepthPrepass.vert", "Assets/Shaders/DepthPrepass.frag");
    m_DepthDisplay = new Shader("Assets/Shaders/DepthDisplay.vert", "Assets/Shaders/DepthDisplay.frag");

    m_Lighting = new Shader("Assets/Shaders/Lighting.vert", "Assets/Shaders/Lighting.frag");
    m_LightingForward = new Shader("Assets/Shaders/Lighting.vert", "Assets/Shaders/LightingNoCheck.frag");
    m_LightCulling = new Shader("Assets/Shaders/LightCulling.cmpt");

    m_FSQ = new Shader("Assets/Shaders/FSQ.vert", "Assets/Shaders/FSQ.frag");
    m_FlatShader = new Shader("Assets/Shaders/FlatShader.vert", "Assets/Shaders/FlatShader.frag");

    m_Sponza = new Model("Assets/Models/Sponza/Sponza.gltf");
    m_Sphere = new Model("Assets/Models/sphere.obj");

    InitScene();

    while (!glfwWindowShouldClose(m_Window))
    {
        UpdateImGUI();

        glClearColor(0.55f, 0.5f, 0.25f, 1.0f);

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        processInput(m_Window);

        if (moveLights)
            Update();

        glm::mat4 projection = m_Camera.perspective(ScreenSize.x, ScreenSize.y);
        glm::mat4 view = m_Camera.view();

        // Sponza model matrix
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.1f));

        // Depth Prepass
        glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        m_DepthPass->Activate();
        m_DepthPass->SetMat4("projection", projection);
        m_DepthPass->SetMat4("view", view);
        m_DepthPass->SetMat4("model", model);
        m_Sponza->Draw(*m_DepthPass);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // ---------------------------------
        
        // Light culling in compute shader
        m_LightCulling->Activate();
        m_LightCulling->SetMat4("projection", projection);
        m_LightCulling->SetMat4("view", view);
        m_LightCulling->SetInt("numLights", numLights);
        m_LightCulling->SetVec2("zPlanes", glm::vec2(m_Camera.n, m_Camera.f));
        glUniform2iv(glGetUniformLocation(m_LightCulling->ID, "screenSize"), 1, &ScreenSize[0]);

        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        m_LightCulling->SetInt("depthMap", 10);
        
        m_LightBuffer->SetData();
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_Visibility);

        glDispatchCompute(computeWorkX, computeWorkY, 1);

        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_2D, 0);
        // ---------------------------------

        glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Regular scene display
        if (displayMode == 0)
        {
            if (!useForward)
            {
                m_Lighting->Activate();
                m_Lighting->SetMat4("projection", projection);
                m_Lighting->SetMat4("view", view);
                m_Lighting->SetMat4("model", model);
                m_Lighting->SetBool("debugDisplay", debugLight);
                m_Lighting->SetVec3("viewPos", m_Camera.cameraPos);
                m_Lighting->SetInt("xAmount", computeWorkX);
                m_Lighting->SetInt("numLights", numLights);

                m_Sponza->Draw(*m_Lighting);
            }

            else
            {
                m_LightingForward->Activate();
                m_LightingForward->SetMat4("projection", projection);
                m_LightingForward->SetMat4("view", view);
                m_LightingForward->SetMat4("model", model);
                m_LightingForward->SetBool("debugDisplay", debugLight);
                m_LightingForward->SetVec3("viewPos", m_Camera.cameraPos);
                m_LightingForward->SetInt("numLights", numLights);

                m_Sponza->Draw(*m_LightingForward);
            }

            if (displayLights)
            {
                m_FlatShader->Activate();
                m_FlatShader->SetMat4("projection", projection);
                m_FlatShader->SetMat4("view", view);

                for (int i = 0; i < numLights; ++i)
                {
                    // Sphere model matrix
                    glm::mat4 sphereModel = glm::mat4(1.0f);
                    sphereModel = glm::translate(sphereModel, glm::vec3(m_LightBuffer->GetData().position[i]));
                    sphereModel = glm::scale(sphereModel, glm::vec3(0.005f));

                    m_FlatShader->SetMat4("model", sphereModel);
                    m_FlatShader->SetVec3("color", glm::vec3(m_LightBuffer->GetData().colorRadius[i]));

                    m_Sphere->Draw(*m_FlatShader);

                    if (displayLightRanges)
                    {
                        sphereModel = glm::scale(sphereModel, glm::vec3(m_LightBuffer->GetData().colorRadius[i].w));

                        m_FlatShader->SetMat4("model", sphereModel);
                        m_FlatShader->SetVec3("color", glm::vec3(m_LightBuffer->GetData().colorRadius[i]));
                        m_Sphere->Draw(*m_FlatShader, GL_LINES);
                    }
                }
            }
        }

        // Display depth
        else if (displayMode == 1)
        {
            m_DepthDisplay->Activate();
            m_DepthDisplay->SetMat4("projection", projection);
            m_DepthDisplay->SetMat4("view", view);
            m_DepthDisplay->SetMat4("model", model);
            m_DepthDisplay->SetFloat("nearP", m_Camera.n);
            m_DepthDisplay->SetFloat("farP", m_Camera.f);

            m_Sponza->Draw(*m_DepthDisplay);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // ---------------------------------
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_FSQ->Activate();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sceneTexture);

        DrawQuad();

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(m_Window);
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (!ImGui::IsItemActive() && ImGui::TempInputIsActive(ImGui::GetActiveID()))
    {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            m_Camera.UpdateCameraPos(CameraDirection::FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            m_Camera.UpdateCameraPos(CameraDirection::BACKWARDS, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            m_Camera.UpdateCameraPos(CameraDirection::LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            m_Camera.UpdateCameraPos(CameraDirection::RIGHT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_GRAVE_ACCENT) == GLFW_PRESS)
        m_Camera.rotateCamera = !m_Camera.rotateCamera;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    m_Camera.UpdateCameraDir(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    m_Camera.UpdateCameraZoom(static_cast<float>(yoffset));
}