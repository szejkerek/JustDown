#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glad/gl.h"
#include "Texture.h"
#include <memory>

struct Vertex {
    float x, y, z;
};

struct TexCoord {
    float u, v;
};

struct Normal {
    float x, y, z;
};

struct Face {
    int v[3], t[3], n[3];
};

enum ModelType
{
    Colored = 0,
    Textured = 1,
    DoubleTextured = 2
};


class Model {
public:
    std::vector<Vertex> vertices;
    std::vector<TexCoord> texCoords;
    std::vector<Normal> normals;
    std::vector<Face> faces;
    GLuint VAO = 0, VBO = 0;
    std::shared_ptr<Texture> texture0 = nullptr; 
    std::shared_ptr<Texture> texture1 = nullptr;
    ModelType modelType;

    glm::vec3 position = glm::vec3(0.0f); // Position of the model
    glm::vec3 rotation = glm::vec3(0.0f); // Rotation (in degrees)
    glm::vec3 scale = glm::vec3(1.0f);    // Scale of the model (default is 1.0 for uniform scaling)

    Model() = default;

    bool loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open OBJ file: " << filename << std::endl;
            return false;
        }

        std::cout << "Loading model from file: " << filename << std::endl;

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream ss(line);
            std::string prefix;
            ss >> prefix;

            if (prefix == "v") {
                Vertex v;
                ss >> v.x >> v.y >> v.z;
                vertices.push_back(v);
            }
            else if (prefix == "vt") {
                TexCoord tc;
                ss >> tc.u >> tc.v;
                texCoords.push_back(tc);
            }
            else if (prefix == "vn") {
                Normal n;
                ss >> n.x >> n.y >> n.z;
                normals.push_back(n);
            }
            else if (prefix == "f") {
                Face f = {};
                for (int i = 0; i < 3; ++i) {
                    std::string vertexData;
                    ss >> vertexData;

                    std::istringstream vertexStream(vertexData);
                    std::string index;

                    if (std::getline(vertexStream, index, '/')) {
                        f.v[i] = std::stoi(index) - 1;
                    }

                    if (std::getline(vertexStream, index, '/')) {
                        f.t[i] = !index.empty() ? std::stoi(index) - 1 : -1;
                    }

                    if (std::getline(vertexStream, index)) {
                        f.n[i] = !index.empty() ? std::stoi(index) - 1 : -1;
                    }
                }
                faces.push_back(f);
            }
        }

        file.close();
        setupBuffers();

       

        return true;
    }

    void setupBuffers() {
        std::cout << "Setting up buffers..." << std::endl;

        std::vector<float> vertexData;
        for (const auto& face : faces) {
            for (int i = 0; i < 3; ++i) {
                const auto& v = vertices[face.v[i]];
                vertexData.push_back(v.x);
                vertexData.push_back(v.y);
                vertexData.push_back(v.z);

                if (face.t[i] >= 0 && face.t[i] < texCoords.size()) {
                    const auto& tc = texCoords[face.t[i]];
                    vertexData.push_back(tc.u);
                    vertexData.push_back(tc.v);
                }
                else {
                    vertexData.push_back(0.0f);
                    vertexData.push_back(0.0f);
                }

                if (face.n[i] >= 0 && face.n[i] < normals.size()) {
                    const auto& n = normals[face.n[i]];
                    vertexData.push_back(n.x);
                    vertexData.push_back(n.y);
                    vertexData.push_back(n.z);
                }
                else {
                    vertexData.push_back(0.0f);
                    vertexData.push_back(0.0f);
                    vertexData.push_back(0.0f);
                }
            }
        }

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);

        std::cout << "Buffers setup complete." << std::endl;
    }

    void render(Shader& shaderProgram) const {
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, position);
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        modelMatrix = glm::scale(modelMatrix, scale);

        shaderProgram.setMat4("transform", modelMatrix);

        if (texture0 && texture0->isLoaded) {
            shaderProgram.setInt("texture1", 0);
            texture0->bind(0);
        }

        if (texture1 && texture1->isLoaded) {
            shaderProgram.setInt("texture2", 1);
            texture1->bind(1);
        }

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(faces.size() * 3));
        glBindVertexArray(0);

        if (texture0 && texture0->isLoaded) {
            texture0->unbind();
        }
        if (texture1 && texture1->isLoaded) {
            texture1->unbind();
        }
    }


    ~Model() {
        if (VAO) {
            glDeleteVertexArrays(1, &VAO);
        }
        if (VBO) {
            glDeleteBuffers(1, &VBO);
        }
    }

    void setTexture(int pos, const std::string& path) {
        if (pos == 0) {
            texture0 = std::make_unique<Texture>(path, GL_TEXTURE_2D);
        }
        else if (pos == 1) {
            texture1 = std::make_unique<Texture>(path, GL_TEXTURE_2D);
        }
        else {
            std::cout << "Wrong texture pos" << std::endl;
        }

        if (texture0 && texture0->isLoaded && texture1 && texture1->isLoaded) {
            modelType = DoubleTextured;
        }
        else if ((texture0 && texture0->isLoaded) || (texture1 && texture1->isLoaded)) {
            modelType = Textured;
        }
        else {
            modelType = Colored;
        }
    }

    void setPosition(float x, float y, float z) {
        position = glm::vec3(x, y, z);
    }

    void setRotation(float pitch, float yaw, float roll) {
        rotation = glm::vec3(pitch, yaw, roll);
    }

    void setScale(float x, float y, float z) {
        scale = glm::vec3(x, y, z);
    }
};
