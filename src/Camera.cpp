#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Camera::Camera(glm::vec3 startPosition, glm::vec3 startFront, glm::vec3 startUp)
    : position(startPosition), front(startFront), up(startUp), freeFlyMode(false),
    yaw(-90.0f), pitch(0.0f), lastX(400), lastY(300), firstMouse(true), lastFrame(0.0f),
    keyPressed(false), savedPosition(startPosition), savedFront(startFront) {
}

void Camera::processInput(GLFWwindow* window, float deltaTime) {
    float cameraSpeed = 2.5f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS && !keyPressed) {
        freeFlyMode = !freeFlyMode;
        std::cout << "Flymode: " << freeFlyMode << std::endl;
        if (!freeFlyMode) {
            position = savedPosition;
            front = savedFront;
        }
        else {
            savedPosition = position;
            savedFront = front;
        }

        keyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_RELEASE) {
        keyPressed = false;
    }

    if (freeFlyMode) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            position += cameraSpeed * front;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            position -= cameraSpeed * front;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            position -= glm::normalize(glm::cross(front, up)) * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            position += glm::normalize(glm::cross(front, up)) * cameraSpeed;
    }
    
}

void Camera::mouseCallback(double xpos, double ypos) {
    if (!freeFlyMode) return;

    if (firstMouse) {
        lastX = static_cast<float>(xpos);  // Cast xpos to float
        lastY = static_cast<float>(ypos);  // Cast ypos to float
        firstMouse = false;
    }

    float xoffset = static_cast<float>(xpos - lastX);  // Cast xpos - lastX to float
    float yoffset = static_cast<float>(lastY - ypos);  // Cast lastY - ypos to float
    lastX = static_cast<float>(xpos);  // Cast xpos to float
    lastY = static_cast<float>(ypos);  // Cast ypos to float

    const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(direction);
}


glm::mat4 Camera::getViewMatrix() const {
    if (freeFlyMode)
        return glm::lookAt(position, position + front, up);
    return glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}
