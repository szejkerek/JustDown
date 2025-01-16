#pragma once

#include <GLFW/include/GLFW/glfw3.h>
#include <glm/glm.hpp>

class Camera {
public:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    bool freeFlyMode;
    bool showOnlyColliders;
    float yaw;
    float pitch;
    float lastX;
    float lastY;
    bool firstMouse;
    float lastFrame;

    Camera(glm::vec3 startPosition, glm::vec3 startFront, glm::vec3 startUp);
    Camera(const Camera& other)
        : position(other.position),
        front(other.front),
        up(other.up),
        freeFlyMode(other.freeFlyMode),
        showOnlyColliders(other.showOnlyColliders),
        yaw(other.yaw),
        pitch(other.pitch),
        lastX(other.lastX),
        lastY(other.lastY),
        firstMouse(other.firstMouse),
        lastFrame(other.lastFrame),
        savedPosition(other.savedPosition),
        savedFront(other.savedFront),
        keyPressed(other.keyPressed) {
    }

    void processInput(GLFWwindow* window, float deltaTime);
    void mouseCallback(double xpos, double ypos);
    glm::mat4 getViewMatrix() const;

private:
    glm::vec3 savedPosition;
    glm::vec3 savedFront;
    bool keyPressed;
};

