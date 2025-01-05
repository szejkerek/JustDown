#include "Scene.h"
#include "Skybox.h"
#include <stb_image.h>  // Make sure stb_image is included for texture loading

// Constructor to initialize the shaders and skybox shader
Scene::Scene(glm::mat4 projection, Camera& cam)
    : texturedShader(std::make_shared<Shader>(
        "src/Shaders/TexturedShader/VertexShader.vs",
        "src/Shaders/TexturedShader/FragmentShader.fs")),
    coloredShader(std::make_shared<Shader>(
        "src/Shaders/ColoredShader/VertexShader.vs",
        "src/Shaders/ColoredShader/FragmentShader.fs")),
    projection(projection)
{
    currentCam = std::make_shared<Camera>(cam);

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
        // Trim whitespace around the line
        if (line.empty()) continue;  // Skip empty lines

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
                lineStream >> skyboxTextures[i];
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

void Scene::render() const
{
    // Render the skybox first to ensure it is behind everything
    if (skybox)
    {
        glDepthFunc(GL_LEQUAL);  // Ensure skybox is rendered first, behind everything else
        glDisable(GL_DEPTH_TEST);  // Disable depth test to render skybox at the farthest distance

        skyboxShader->use();
        skyboxShader->setMat4("projection", projection);
        skyboxShader->setMat4("view", glm::mat4(glm::mat3(currentCam->getViewMatrix())));  // Remove translation from view matrix
        skybox->render();

        glEnable(GL_DEPTH_TEST);  // Re-enable depth testing for the rest of the scene
    }

    // Then render the scene models
    for (const auto& model : sceneModels)
    {
        model.render(GetShader(model));
    }
}

std::shared_ptr<Shader> Scene::GetShader(const Model& model) const
{
    std::shared_ptr<Shader> shaderToUse;

    switch (model.modelType)
    {
    case Colored:
        shaderToUse = coloredShader;
        break;
    case Textured:
    case DoubleTextured:
        shaderToUse = texturedShader;
        break;
    default:
        shaderToUse = coloredShader;
        break;
    }

    shaderToUse->use();
    shaderToUse->setMat4("projection", projection);
    shaderToUse->setMat4("view", currentCam->getViewMatrix());

    return shaderToUse;
}

void Scene::setSkybox(const std::vector<std::string>& skyboxTextures)
{
    skybox = std::make_shared<Skybox>(skyboxTextures);
}
