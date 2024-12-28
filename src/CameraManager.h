#pragma once
class CameraManager
{
public:
	// Camera parameters
    static glm::vec3 cameraPos;
    static glm::vec3 cameraFront;
    static glm::vec3 cameraUp;

    static glm::vec3 savedCameraPos;
    static glm::vec3 savedCameraFront;

    static bool freeFlyMode; // Track the current mode
    static bool keyPressed; // Prevent holding the key from toggling repeatedly
           
    static float yaw;
    static float pitch;
    static float lastX;
    static float lastY;
    static bool firstMouse; // First mouse movement check
           

	
    static void Init()
    {
        cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
        cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
        cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

        savedCameraPos = cameraPos; // To save the fixed position
        savedCameraFront = cameraFront;

        freeFlyMode = false; // Track the current mod
        keyPressed = false;  // Prevent holding the k

        yaw = -90.0f; // Initial yaw
        pitch = 0.0f; // Initial pitch
        lastX = 400;
        lastY = 300; // Initial mouse p
        firstMouse = true; // First mouse movement ch


    }

    void processInput(GLFWwindow* window, float deltaTime)
    {
        const float cameraSpeed = 2.5f * deltaTime; // Adjust camera speed

        // Toggle free-fly mode with the "O" key
        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS && !keyPressed)
        {
            freeFlyMode = !freeFlyMode;

            if (!freeFlyMode)
            {
                // Restore the saved fixed camera position
                cameraPos = savedCameraPos;
                cameraFront = savedCameraFront;
            }
            else
            {
                // Save the current camera position for returning later
                savedCameraPos = cameraPos;
                savedCameraFront = cameraFront;
            }

            keyPressed = true;
        }
        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_RELEASE)
        {
            keyPressed = false;
        }

        // Movement controls only in free-fly mode
        if (freeFlyMode)
        {
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                cameraPos += cameraSpeed * cameraFront;
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                cameraPos -= cameraSpeed * cameraFront;
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        }

        // Toggle Wireframe mode while holding the "P" key
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe mode
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Default (filled) mode
        }
    }


    static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
    {
        if (!freeFlyMode) return; // Mouse input is only for free-fly mode

        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // Reversed: y-coordinates range from bottom to top
        lastX = xpos;
        lastY = ypos;

        const float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        // Constrain pitch to avoid screen flipping
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        // Update camera direction
        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(direction);
    }

    static glm::mat4 GetView()
    {
        glm::mat4 view;
        if (freeFlyMode)
        {
            view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        }
        else
        {
            // Fixed camera view
            view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        }
        return view;
    }

};