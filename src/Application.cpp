#include <glad/gl.h>
#include <GLFW/include/GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include "Camera.h"
#include "Context.h"
#include "Model.h"
#include "Texture.h"
#include "Scene.h"

float deltaTime = 0.0f;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

void processInput_callback(GLFWwindow* window)
{
    processInput(window);
    camera.processInput(window, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    camera.mouseCallback(xpos, ypos);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main()
{
    if (!initGLFW())
        return -1;

    GLFWwindow* window = createWindow(800, 600, "Load OBJ Model");
    if (!window)
        return -1;

    if (!initGLAD())
        return -1;

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    Scene scene(projection);
    scene.loadFromFile("Data/Level0.scene");

    while (!glfwWindowShouldClose(window))
    {
        processInput_callback(window);
        deltaTime = calculateDeltatime();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        scene.render(camera);
        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    glfwTerminate();
    return 0;
}
