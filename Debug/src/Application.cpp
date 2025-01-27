#include <glad/gl.h>
#include <GLFW/include/GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include "Camera.h"
#include "Context.h"
#include "Model.h"
#include "Texture.h"
#include "Scene.h"
#include "Player.h"
#include "../PostProcess.h"

const float width = 800.0f;
const float height = 600.0f;

bool postProcessStoped;
float deltaTime = 0.0f;
std::shared_ptr <Camera> camera = std::make_shared<Camera>(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
Player player(camera);

void processInput_callback(GLFWwindow* window)
{
    postProcessStoped = processInput(window);
    camera->processInput(window, deltaTime);
    player.processInput(window, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    camera->mouseCallback(xpos, ypos);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main()
{
    if (!initGLFW())
        return -1;

    GLFWwindow* window = createWindow(width, height, "Load OBJ Model");
    if (!window)
        return -1;

    if (!initGLAD())
        return -1;

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), width / height, 0.1f, 100.0f);

    Scene scene(projection);
    scene.loadFromFile("Data/Level0.scene");

    std::shared_ptr<Shader> lightShader = std::make_shared<Shader>("src/Shaders/LightShader/VertexShader.vs",
        "src/Shaders/LightShader/FragmentShader.fs");


    player.playerModel.loadFromFile("Data/Geometry/cube.obj");
    player.playerModel.setupBuffers();
    player.playerModel.setScale(0.2f, 0.7f, 0.1f);
    PostProcess postProcess;
    postProcess.Init(width, height);

    

    while (!glfwWindowShouldClose(window))
    {
        deltaTime = calculateDeltatime();
        processInput_callback(window);

        if (!camera->freeFlyMode)
            camera->position = player.playerModel.position + glm::vec3(0.0f, player.playerModel.scale.y+0.4f, 0.0f);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if(postProcessStoped)
            postProcess.BeginRender();

        scene.update(deltaTime);
        scene.render(camera);

        player.applyPhysics(deltaTime, scene);
        player.render(scene);
        if (postProcessStoped) 
            postProcess.ApplyBloom(0.6f, 0.8f);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
