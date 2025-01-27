#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "Model.h"
#include "Model.h"
#include "Skybox.h"
#include "CollisionResult.h"

class Scene
{
public:
    Scene(glm::mat4 projection);
    bool loadFromFile(const std::string& filePath);
    void update(float deltaTime);
    CollisionResult checkPlayerCollision(Model& playerModel);
    void render(std::shared_ptr <Camera>& camera) const;

    std::shared_ptr<Shader> GetShader(const Model& model, std::shared_ptr <Camera>& camera) const;
    void setSkybox(const std::vector<std::string>& skyboxTextures);

private:

    glm::mat4 projection;
    std::vector<Model> sceneModels;
    std::shared_ptr<Shader> texturedShader;
    std::shared_ptr<Shader> doubletexturedShader;
    std::shared_ptr<Shader> coloredShader;
    std::shared_ptr<Shader> skyboxShader;
    std::shared_ptr<Skybox> skybox;
};

Scene::Scene(glm::mat4 projection)
    : texturedShader(std::make_shared<Shader>(
        "src/Shaders/LightsTexturedShader/VertexShader.vs",
        "src/Shaders/LightsTexturedShader/FragmentShader.fs")),
    coloredShader(std::make_shared<Shader>(
        "src/Shaders/LightsShader/VertexShader.vs",
        "src/Shaders/LightsShader/FragmentShader.fs")),
    doubletexturedShader(std::make_shared<Shader>(
        "src/Shaders/DoubleTexturedShader/VertexShader.vs",
        "src/Shaders/DoubleTexturedShader/FragmentShader.fs")),
    projection(projection)
{

    skyboxShader = std::make_shared<Shader>(
        "src/Shaders/SkyboxShader/VertexShader.vs",
        "src/Shaders/SkyboxShader/FragmentShader.fs");
}

bool Scene::loadFromFile(const std::string& filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        std::cerr << "Failed to open scene file: " << filePath << std::endl;
        return false;
    }
    
    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream lineStream(line);
        std::string command;
        lineStream >> command;
        
        if (command == "Model")
        {   
            

            Model model;
            std::string modelPath;
            lineStream >> modelPath;

            if (!model.loadFromFile(modelPath))
            {
                std::cerr << "Failed to load model: " << modelPath << std::endl;
                continue;
            }

            sceneModels.push_back(std::move(model));
            //sceneModels.back().setupBuffers();
        }
        else if (command == "Texture0")
        {
            if (sceneModels.empty())
            {
                std::cerr << "Texture0 command before any Model command." << std::endl;
                continue;
            }

            std::string texturePath;
            lineStream >> texturePath;
            sceneModels.back().setTexture(0, texturePath);
        }
        else if (command == "Texture1")
        {
            if (sceneModels.empty())
            {
                std::cerr << "Texture1 command before any Model command." << std::endl;
                continue;
            }

            std::string texturePath;
            lineStream >> texturePath;
            sceneModels.back().setTexture(1, texturePath);
        }
        else if (command == "Position")
        {
            if (sceneModels.empty())
            {
                std::cerr << "Position command before any Model command." << std::endl;
                continue;
            }

            float x, y, z;
            lineStream >> x >> y >> z;
            sceneModels.back().setPosition(x, y, z);
        }
        else if (command == "Rotation")
        {
            if (sceneModels.empty())
            {
                std::cerr << "Rotation command before any Model command." << std::endl;
                continue;
            }

            float pitch, yaw, roll;
            lineStream >> pitch >> yaw >> roll;
            sceneModels.back().setRotation(pitch, yaw, roll);
        }
        else if (command == "Scale")
        {
            if (sceneModels.empty())
            {
                std::cerr << "Scale command before any Model command." << std::endl;
                continue;
            }

            float sx, sy, sz;
            lineStream >> sx >> sy >> sz;
            sceneModels.back().setScale(sx, sy, sz);
        }
        else if (command == "Skybox")
        {
            std::vector<std::string> skyboxTextures(6);
            // Read the next 6 texture paths for the skybox
            for (int i = 0; i < 6; ++i)
            {
                std::getline(file, line);
                std::istringstream lineStreamTMP(line);
                lineStreamTMP >> skyboxTextures[i];
            }
            setSkybox(skyboxTextures);  // Set the skybox textures
        }
        else
        {
            std::cerr << "Unknown command: " << command << std::endl;
        }
    }

    for (auto& model : sceneModels)
    {
        model.setupBuffers();
    }


    file.close();
    return true;
}

void Scene::update(float deltaTime = 0.0f)
{

    if (!Model::checkCollision(sceneModels[0].getTransformedAABB(), sceneModels[1].getTransformedAABB()))
    {
        sceneModels[0].setPosition(sceneModels[0].position.x, sceneModels[0].position.y - 1.0 * deltaTime, sceneModels[0].position.z);
    }


}

CollisionResult Scene::checkPlayerCollision(Model& playerModel) {
    CollisionResult result = { false, glm::vec3(0.0f) };

    for (Model& model : sceneModels) {
        if (Model::checkCollision(model.getTransformedAABB(), playerModel.getTransformedAABB())) {
            result.collided = true;

            // Oblicz normaln¹ kolizji (np. poprzez ró¿nicê pozycji modeli)
            glm::vec3 direction = playerModel.position - model.position;
            result.collisionNormal = glm::normalize(direction);

            return result; // Wychodzimy przy pierwszej kolizji
        }
    }
    return result;
}

void Scene::render(std::shared_ptr <Camera>& camera) const
{
    // Render the skybox first to ensure it is behind everything
    if (skybox)
    {
        glDepthFunc(GL_LEQUAL);  // Ensure skybox is rendered first, behind everything else
        glDisable(GL_DEPTH_TEST);  // Disable depth test to render skybox at the farthest distance

        skyboxShader->use();
        skyboxShader->setMat4("projection", projection);
        skyboxShader->setMat4("view", glm::mat4(glm::mat3(camera->getViewMatrix())));  // Remove translation from view matrix
        skybox->render(skyboxShader);

        glEnable(GL_DEPTH_TEST);  // Re-enable depth testing for the rest of the scene
    }


    for (const auto& model : sceneModels)
    {
        if (camera->showOnlyColliders) {
            model.renderAABB(projection, camera->getViewMatrix(), coloredShader);
        }
        else
        {
            model.render(GetShader(model,camera), camera->position);     
        }
    }
}

std::shared_ptr<Shader> Scene::GetShader(const Model& model, std::shared_ptr <Camera>& camera) const
{
    std::shared_ptr<Shader> shaderToUse;

    // Determine the shader to use based on the model type
    switch (model.modelType)
    {
    case Colored:
        shaderToUse = coloredShader;
        break;
    case Textured:
        shaderToUse = texturedShader;
        break;
    case DoubleTextured:
        shaderToUse = doubletexturedShader;
        break;
    default:
        shaderToUse = coloredShader;
        break;
    }

    // Common setup for the shader
    shaderToUse->use();
    shaderToUse->setMat4("projection", projection);
    shaderToUse->setMat4("view", camera->getViewMatrix());

    return shaderToUse;
}

void Scene::setSkybox(const std::vector<std::string>& skyboxTextures)
{
    skybox = std::make_shared<Skybox>(skyboxTextures);
}

