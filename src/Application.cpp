#include <glad/gl.h>
#include <GLFW/include/GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "Shader.h"
#include "Camera.h"
#include "Context.h"
#include "Model/Model.h"
#include "Texture.h"



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
    
    Shader texturedShader("src/Shaders/TexturedShader/VertexShader.vs", "src/Shaders/TexturedShader/FragmentShader.fs");
    Shader coloredShader("src/Shaders/ColoredShader/VertexShader.vs", "src/Shaders/ColoredShader/FragmentShader.fs");

    Model pistolModel;
    pistolModel.loadFromFile("Data/Pistol/Pistol.obj");
    pistolModel.setPosition(-0.1,0,2.5);
    pistolModel.setRotation(0,90,0);

    Model pistolModel2;
    pistolModel2.loadFromFile("Data/Pistol/Pistol.obj");
    pistolModel2.setPosition(0.3, 0, 2.5);
    pistolModel2.setRotation(0, -45, 0);

    Texture texture("Data/Pistol/pistol.png", GL_TEXTURE_2D);


    while (!glfwWindowShouldClose(window))
    {
        processInput_callback(window);
        deltaTime = calculateDeltatime();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        glm::mat4 view = camera.getViewMatrix();


        texturedShader.use();
        texturedShader.setMat4("projection", projection);
        texturedShader.setMat4("view", view);
        texturedShader.setInt("texture1", 0);
        texture.bind(0);
        pistolModel.render(texturedShader);


        coloredShader.use();
        coloredShader.setMat4("projection", projection);
        coloredShader.setMat4("view", view);

        pistolModel2.render(coloredShader);

        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    glfwTerminate();
    return 0;
}
