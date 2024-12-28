#pragma once

#include <glm/glm.hpp>
#include <GLFW/include/GLFW/glfw3.h>

class Camera {
public:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    bool freeFlyMode;
    float yaw;
    float pitch;
    float lastX;
    float lastY;
    bool firstMouse;
    float lastFrame;

    Camera(glm::vec3 startPosition, glm::vec3 startFront, glm::vec3 startUp);

    void processInput(GLFWwindow* window, float deltaTime);
    void mouseCallback(double xpos, double ypos);
    glm::mat4 getViewMatrix() const;

private:
    glm::vec3 savedPosition;
    glm::vec3 savedFront;
    bool keyPressed;
};

