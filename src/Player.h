class Player {
public:
    Model playerModel;
    std::shared_ptr<Camera> camera;
    glm::vec3 velocity;
    bool isGrounded = true;
    float speed = 2.5f; // Movement speed
    float jumpStrength = 5.0f; // Jumping force
    float gravity = -5.0f; // Gravitational acceleration
    float groundY = 0.0f; // Y-coordinate for the ground level
    float health = 100.0f; // Player health
    float fallStartHeight = 0.0f; // Track the height when the fall starts
    float fallStartDamageHeight = 1.0f;

    void processInput(GLFWwindow* window, float deltaTime);
    void applyPhysics(float deltaTime, Scene& scene);
    void ApplyFallDamage();
    void render(Scene& scene);

    Player(std::shared_ptr<Camera>& camera);
    ~Player();
};

Player::Player(std::shared_ptr<Camera>& camera) : camera(camera), velocity(0.0f) {}

Player::~Player() {}

void Player::processInput(GLFWwindow* window, float deltaTime) {
    if (camera->freeFlyMode)
        return;

    glm::vec3 inputVelocity(0.0f);

    // Movement controls
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        glm::vec3 forward = glm::vec3(camera->front.x, 0.0f, camera->front.z);
        if (glm::length(forward) > 0.0f) {
            forward = glm::normalize(forward);
        }
        inputVelocity += forward;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        glm::vec3 backward = glm::vec3(camera->front.x, 0.0f, camera->front.z);
        if (glm::length(backward) > 0.0f) {
            backward = glm::normalize(backward);
        }
        inputVelocity -= backward;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        glm::vec3 left = glm::cross(camera->front, camera->up);
        left = glm::vec3(left.x, 0.0f, left.z);
        if (glm::length(left) > 0.0f) {
            left = glm::normalize(left);
        }
        inputVelocity -= left;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        glm::vec3 right = glm::cross(camera->front, camera->up);
        right = glm::vec3(right.x, 0.0f, right.z);
        if (glm::length(right) > 0.0f) {
            right = glm::normalize(right);
        }
        inputVelocity += right;
    }

    // Player rotation
    if (glm::length(glm::vec2(camera->front.x, camera->front.z)) > 0.0f) {
        float angle = atan2(camera->front.x, camera->front.z);
        playerModel.rotation.y = glm::degrees(angle);
    }

    velocity.x = inputVelocity.x;
    velocity.z = inputVelocity.z;

    // Jump control
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && isGrounded) {
        velocity.y = jumpStrength;
        isGrounded = false;
        fallStartHeight = playerModel.position.y;
        //std::cout << "Start to Fall: " << fallStartHeight << "\n";
    }
}
void Player::applyPhysics(float deltaTime, Scene& scene) {
    if (camera->freeFlyMode)
        return;

    if(!isGrounded) velocity.y += gravity * deltaTime;

    playerModel.position += velocity * deltaTime;

    CollisionResult collision = scene.checkPlayerCollision(playerModel);
    if (collision.collided) {

        if (collision.collisionNormal.y > 0.f) {
            // Player lands
            if (!isGrounded) {
                ApplyFallDamage();
            }
            isGrounded = true;
            velocity.y = 0.0f;
        }
        else {
            playerModel.position -= velocity * deltaTime;
            velocity.x = 0.0f;
            velocity.z = 0.0f;
        }
    }

    if (playerModel.position.y <= groundY) {
        if (!isGrounded) {
            ApplyFallDamage();
        }

        playerModel.position.y = groundY;
        velocity.y = 0.0f;
        isGrounded = true;
    }
    else if (isGrounded && !collision.collided) {
        // Start of the fall
        isGrounded = false;
        fallStartHeight = playerModel.position.y;
        //std::cout << "Start to Fall: " << fallStartHeight << "\n";
    }
}

void Player::ApplyFallDamage()
{
    float fallHeight = fallStartHeight - playerModel.position.y;
    std::cout << "Fall height: " << fallHeight << "f\n";

    // Optionally apply damage for a dangerous fall
    if (fallHeight > fallStartDamageHeight) {
        health -= (fallHeight - fallStartDamageHeight) * 10.0f; // Reduce health for dangerous falls
        std::cout << "Health reduced to: " << health << "\n";
    }

    if (health <= 0.f)
    {
        glfwTerminate();
        return;
    }
        
}

void Player::render(Scene& scene) {
    playerModel.render(scene.GetShader(playerModel, camera), camera->position);
}
