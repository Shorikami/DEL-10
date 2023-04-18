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

unsigned numLights = 200;
float nearP = 0.1f;
float farP = 300.0f;
glm::ivec2 ScreenSize(1280, 720);

GLFWwindow* m_Window;

GLuint quadVAO = 0, quadVBO = 0;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

void InitGLFW();
void InitScene();
void InitLights();

void Update();
void DrawQuad();

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = 1280.0f / 2.0f;
float lastY = 720.0f / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

DEL10::UniformBuffer<DEL10::Light>* m_LightBuffer;
DEL10::UniformBuffer<DEL10::VisibilityIdx>* m_Visibility;

GLuint computeWorkX = 0, computeWorkY = 0;

#endif