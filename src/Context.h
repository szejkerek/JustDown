#pragma once

#include <GLFW/include/GLFW/glfw3.h>

bool initGLFW();
GLFWwindow* createWindow(int width, int height, const char* title);
bool initGLAD();
void processInput(GLFWwindow* window);
float calculateDeltatime();

