#pragma once
#include <glm/ext/vector_float3.hpp>

struct CollisionResult {
    bool collided;
    glm::vec3 collisionNormal; // Kierunek kolizji
};
