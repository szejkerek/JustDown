#pragma once
#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

class Player {
public:
    std::shared_ptr <Camera> camera;
    glm::vec3 position;
    glm::vec3 velocity;
    bool isGrounded = true;
    float speed = 2.5f; // Movement speed
    float jumpStrength = 10.0f; // Jumping force
    float gravity = -5.0f; // Gravitational acceleration
    float groundY = 0.0f; // Y-coordinate for the ground level

    void processInput(GLFWwindow* window, float deltaTime);
    void applyPhysics(float deltaTime);

    Player(std::shared_ptr <Camera>& camera);
    ~Player();
};

Player::Player(std::shared_ptr <Camera>& camera) : camera(camera), position(0.0f, 0.0f, 0.0f), velocity(0.0f) {}

Player::~Player() {}

void Player::processInput(GLFWwindow* window, float deltaTime) {
    float adjustedSpeed = speed * deltaTime;

    // Movement controls
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        position += adjustedSpeed * camera->front;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        position -= adjustedSpeed * camera->front;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        position -= glm::normalize(glm::cross(camera->front, camera->up)) * adjustedSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        position += glm::normalize(glm::cross(camera->front, camera->up)) * adjustedSpeed;

    // Jump control
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && isGrounded) {
        velocity.y = jumpStrength;
        isGrounded = false;
    }
}

void Player::applyPhysics(float deltaTime) {
    // Apply gravity
    velocity.y += gravity * deltaTime;

    // Update position based on velocity
    position += velocity * deltaTime;

    // Ground collision detection
    if (position.y <= groundY) {
        position.y = groundY;
        velocity.y = 0.0f;
        isGrounded = true;
    }
}
