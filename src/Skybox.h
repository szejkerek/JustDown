#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <memory>
#include "Shader.h"

class Skybox
{
public:
    Skybox(const std::vector<std::string>& faces);
    void render(std::shared_ptr<Shader> shaderProgram) const;
    void loadCubemap(const std::vector<std::string>& faces);

private:
    GLuint VAO, VBO, cubemapTexture;
};
