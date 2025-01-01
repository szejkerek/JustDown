#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "Model.h"

class Scene
{
public:
    Scene();
    bool loadFromFile(const std::string& filePath);
    void render() const;

private:
    std::vector<Model> sceneModels;
    Shader texturedShader;
    Shader coloredShader;
};

Scene::Scene()
    : texturedShader("src/Shaders/TexturedShader/VertexShader.vs", "src/Shaders/TexturedShader/FragmentShader.fs"),
    coloredShader("src/Shaders/ColoredShader/VertexShader.vs", "src/Shaders/ColoredShader/FragmentShader.fs") {
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
        else
        {
            std::cerr << "Unknown command: " << command << std::endl;
        }
    }

    file.close();
    return true;
}

void Scene::render() const
{
    for (const auto& model : sceneModels)
    {
        //model.render();
    }
}
