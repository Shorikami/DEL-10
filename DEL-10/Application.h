#ifndef APPLICATION_H
#define APPLICATION_H

#include "Model.h"
#include "Camera.h"
#include "Shader.h"

#include "UniformMemory.hpp"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#define MAX_LIGHTS 10000

int numLights = 1000;
float nearP = 0.1f;
float farP = 300.0f;
glm::ivec2 ScreenSize(1280, 720);
float globalRadius = 30.0f;

int displayMode = 0;
bool debugLight = false;
bool moveLights = true;
bool displayLights = false;
bool displayLightRanges = false;
bool useForward = false;
float angleOfRotation = 0.0001f;

GLFWwindow* m_Window;

GLuint quadVAO = 0, quadVBO = 0;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

void InitImGUI();
void InitGLFW();
void InitScene();
void InitFBOs();
void InitLights();

void Update();
void UpdateImGUI();
void DrawQuad();

void ReloadShaders();

// camera
Camera m_Camera(glm::vec3(11.0f, 9.0f, -1.0f));
float lastX = 1280.0f / 2.0f;
float lastY = 720.0f / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

DEL10::UniformBuffer<DEL10::Light>* m_LightBuffer;
GLuint m_Visibility;

GLuint computeWorkX = 0, computeWorkY = 0;
GLuint depthFBO, depthRBO;
GLuint depthMap;

GLuint sceneFBO, sceneTexture;

Shader* m_DepthPass, * m_LightCulling, * m_Lighting, * m_LightingForward;
Shader* m_DepthDisplay, * m_FSQ, * m_FlatShader;

Model* m_Sponza;
Model* m_Sphere;

#endif