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
    ~Scene();

    // Load scene description from a text file
    bool loadFromFile(const std::string& filePath);

    // Render all models in the scene (optional, if needed)
    void render() const;

private:
    std::vector<Model> sceneModels;
};

Scene::Scene()
{
}

Scene::~Scene()
{
}

bool Scene::loadFromFile(const std::string& filePath)
{
    //std::ifstream file(filePath);
    //if (!file.is_open())
    //{
    //    std::cerr << "Failed to open scene file: " << filePath << std::endl;
    //    return false;
    //}

    //std::string line;
    //while (std::getline(file, line))
    //{
    //    std::istringstream lineStream(line);
    //    std::string command;
    //    lineStream >> command;

    //    if (command == "Model")
    //    {
    //        Model model;
    //        std::string modelPath;
    //        lineStream >> modelPath;

    //        if (!model.loadFromFile(modelPath))
    //        {
    //            std::cerr << "Failed to load model: " << modelPath << std::endl;
    //            continue;
    //        }

    //        sceneModels.push_back(std::move(model));
    //    }
    //    else if (command == "Texture1")
    //    {
    //        if (sceneModels.empty())
    //        {
    //            std::cerr << "Texture1 command before any Model command." << std::endl;
    //            continue;
    //        }

    //        std::string texturePath;
    //        lineStream >> texturePath;
    //        sceneModels.back().loadTexture(0, texturePath); // Assuming texture index 0
    //    }
    //    else if (command == "Texture2")
    //    {
    //        if (sceneModels.empty())
    //        {
    //            std::cerr << "Texture2 command before any Model command." << std::endl;
    //            continue;
    //        }

    //        std::string texturePath;
    //        lineStream >> texturePath;
    //        sceneModels.back().loadTexture(1, texturePath); // Assuming texture index 1
    //    }
    //    else if (command == "Position")
    //    {
    //        if (sceneModels.empty())
    //        {
    //            std::cerr << "Position command before any Model command." << std::endl;
    //            continue;
    //        }

    //        float x, y, z;
    //        lineStream >> x >> y >> z;
    //        sceneModels.back().setPosition(x, y, z);
    //    }
    //    else if (command == "Rotation")
    //    {
    //        if (sceneModels.empty())
    //        {
    //            std::cerr << "Rotation command before any Model command." << std::endl;
    //            continue;
    //        }

    //        float pitch, yaw, roll;
    //        lineStream >> pitch >> yaw >> roll;
    //        sceneModels.back().setRotation(pitch, yaw, roll);
    //    }
    //    else if (command == "Scale")
    //    {
    //        if (sceneModels.empty())
    //        {
    //            std::cerr << "Scale command before any Model command." << std::endl;
    //            continue;
    //        }

    //        float sx, sy, sz;
    //        lineStream >> sx >> sy >> sz;
    //        sceneModels.back().setScale(sx, sy, sz);
    //    }
    //    else
    //    {
    //        std::cerr << "Unknown command: " << command << std::endl;
    //    }
    //}

    //file.close();
    return true;
}

void Scene::render() const
{
    for (const auto& model : sceneModels)
    {
        //model.render(); // Assuming Model has a render method
    }
}
