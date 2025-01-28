#pragma once

#include <glad/gl.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <memory>
#include "Texture.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
    DoubleTextured = 2,
    Parallax = 3
};

struct AABB {
    glm::vec3 min;
    glm::vec3 max;
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
    std::shared_ptr<Texture> texture2 = nullptr;
    ModelType modelType;

    glm::vec3 position = glm::vec3(0.0f); // Position of the model
    glm::vec3 rotation = glm::vec3(0.0f); // Rotation (in degrees)
    glm::vec3 scale = glm::vec3(1.0f);    // Scale of the model (default is 1.0 for uniform scaling)

    AABB aabb;

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
       // setupBuffers();
        calculateAABB();
       

        return true;
    }

    void calculateAABB() {
        glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
        glm::vec3 max = glm::vec3(std::numeric_limits<float>::lowest());

        // Calculate local-space AABB
        for (const auto& vertex : vertices) {
            min.x = std::min(min.x, vertex.x);
            min.y = std::min(min.y, vertex.y);
            min.z = std::min(min.z, vertex.z);

            max.x = std::max(max.x, vertex.x);
            max.y = std::max(max.y, vertex.y);
            max.z = std::max(max.z, vertex.z);
        }

        aabb = { min, max };
    }

    AABB getTransformedAABB() const {
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, position);
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        modelMatrix = glm::scale(modelMatrix, scale);

        // Compute all 8 vertices of the local-space AABB
        glm::vec3 vertices[8] = {
            glm::vec3(aabb.min.x, aabb.min.y, aabb.min.z),
            glm::vec3(aabb.min.x, aabb.min.y, aabb.max.z),
            glm::vec3(aabb.min.x, aabb.max.y, aabb.min.z),
            glm::vec3(aabb.min.x, aabb.max.y, aabb.max.z),
            glm::vec3(aabb.max.x, aabb.min.y, aabb.min.z),
            glm::vec3(aabb.max.x, aabb.min.y, aabb.max.z),
            glm::vec3(aabb.max.x, aabb.max.y, aabb.min.z),
            glm::vec3(aabb.max.x, aabb.max.y, aabb.max.z),
        };

        glm::vec3 transformedMin = glm::vec3(std::numeric_limits<float>::max());
        glm::vec3 transformedMax = glm::vec3(std::numeric_limits<float>::lowest());

        // Transform each vertex and compute the new AABB
        for (const auto& vertex : vertices) {
            glm::vec3 transformedVertex = glm::vec3(modelMatrix * glm::vec4(vertex, 1.0f));
            transformedMin = glm::min(transformedMin, transformedVertex);
            transformedMax = glm::max(transformedMax, transformedVertex);
        }

        return { transformedMin, transformedMax };
    }

    void setupBuffers() {

        if (modelType == Parallax)
        {

            std::cout << "Setting up buffers... Parallax" << std::endl;

            std::vector<float> vertexData;
            for (const auto& face : faces) {
                glm::vec3 pos[3];
                glm::vec2 tex[3];

                // Pobranie pozycji i wspó³rzêdnych tekstur dla trójk¹ta
                for (int i = 0; i < 3; ++i) {
                    pos[i] = glm::vec3(vertices[face.v[i]].x, vertices[face.v[i]].y, vertices[face.v[i]].z);
                    tex[i] = face.t[i] >= 0 && static_cast<size_t>(face.t[i]) < texCoords.size()
                        ? glm::vec2(texCoords[face.t[i]].u, texCoords[face.t[i]].v)
                        : glm::vec2(0.0f);
                }

                // Obliczanie tangentów i bitangentów
                glm::vec3 edge1 = pos[1] - pos[0];
                glm::vec3 edge2 = pos[2] - pos[0];
                glm::vec2 deltaUV1 = tex[1] - tex[0];
                glm::vec2 deltaUV2 = tex[2] - tex[0];

                float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
                glm::vec3 tangent = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
                glm::vec3 bitangent = f * (-deltaUV2.x * edge1 + deltaUV1.x * edge2);

                // Normalizacja tangentów i bitangentów
                tangent = glm::normalize(tangent);
                bitangent = glm::normalize(bitangent);

                // Dodawanie danych wierzcho³kowych
                for (int i = 0; i < 3; ++i) {
                    const auto& v = vertices[face.v[i]];
                    vertexData.push_back(v.x);
                    vertexData.push_back(v.y);
                    vertexData.push_back(v.z);

                    if (face.t[i] >= 0 && static_cast<size_t>(face.t[i]) < texCoords.size()) {
                        const auto& tc = texCoords[face.t[i]];
                        vertexData.push_back(tc.u);
                        vertexData.push_back(tc.v);
                    }
                    else {
                        vertexData.push_back(0.0f);
                        vertexData.push_back(0.0f);
                    }

                    if (face.n[i] >= 0 && static_cast<size_t>(face.n[i]) < normals.size()) {
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

                    // Tangent i Bitangent
                    vertexData.push_back(tangent.x);
                    vertexData.push_back(tangent.y);
                    vertexData.push_back(tangent.z);

                    vertexData.push_back(bitangent.x);
                    vertexData.push_back(bitangent.y);
                    vertexData.push_back(bitangent.z);
                }
            }

            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

            // Pozycja wierzcho³ka
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            // Wspó³rzêdne tekstur
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);

            // Normalny
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(5 * sizeof(float)));
            glEnableVertexAttribArray(2);

            // Tangenty
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
            glEnableVertexAttribArray(3);

            // Bitangenty
            glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
            glEnableVertexAttribArray(4);

            glBindVertexArray(0);

            std::cout << "Buffers setup complete." << std::endl;
            

            
        }
        else
        {

 

        std::cout << "Setting up buffers..." << std::endl;

        std::vector<float> vertexData;
        for (const auto& face : faces) {
            for (int i = 0; i < 3; ++i) {
                const auto& v = vertices[face.v[i]];
                vertexData.push_back(v.x);
                vertexData.push_back(v.y);
                vertexData.push_back(v.z);

                if (face.t[i] >= 0 && static_cast<size_t>(face.t[i]) < texCoords.size()) {  // Cast to size_t
                    const auto& tc = texCoords[face.t[i]];
                    vertexData.push_back(tc.u);
                    vertexData.push_back(tc.v);
                }
                else {
                    vertexData.push_back(0.0f);
                    vertexData.push_back(0.0f);
                }

                if (face.n[i] >= 0 && static_cast<size_t>(face.n[i]) < normals.size()) {  // Cast to size_t
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


    }

    void render(std::shared_ptr<Shader> shaderProgram, glm::vec3 viewPos) const {
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, position);
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        modelMatrix = glm::scale(modelMatrix, scale);

        shaderProgram->setMat4("transform", modelMatrix);

        if (modelType == Colored || modelType == Textured || modelType == Parallax || modelType == DoubleTextured)
        {
            //glm::vec3 lightColor = glm::vec3(0.4f, 0.8f,0.6f);
           /* lightColor.x = static_cast<float>(sin(glfwGetTime() * 2.0));
            lightColor.y = static_cast<float>(sin(glfwGetTime() * 0.7));
            lightColor.z = static_cast<float>(sin(glfwGetTime() * 1.3));*/

            float distance = 4.f;

            glm::vec3 lightColor = glm::vec3(1.0f, 0.0f, 0.0f);
            glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f); // decrease the influence
            glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);

            shaderProgram->setVec3("viewPos", viewPos);


            shaderProgram->setVec3("light[0].position", distance, 2.0f, distance);
            shaderProgram->setVec3("light[0].ambient", ambientColor);
            shaderProgram->setVec3("light[0].diffuse", diffuseColor);
            shaderProgram->setVec3("light[0].specular", 1.0f, 1.0f, 1.0f);

            lightColor = glm::vec3(0.0f, 1.0f, 0.0f);
            diffuseColor = lightColor * glm::vec3(0.5f); 
            ambientColor = diffuseColor * glm::vec3(0.2f);


            shaderProgram->setVec3("light[1].position", -distance, 2.0f, distance);
            shaderProgram->setVec3("light[1].ambient", ambientColor);
            shaderProgram->setVec3("light[1].diffuse", diffuseColor);
            shaderProgram->setVec3("light[1].specular", 1.0f, 1.0f, 1.0f);

            lightColor = glm::vec3(0.0f, 0.0f, 1.0f);
            diffuseColor = lightColor * glm::vec3(0.5f);
            ambientColor = diffuseColor * glm::vec3(0.2f);

            shaderProgram->setVec3("light[2].position", distance, 2.0f, -distance);
            shaderProgram->setVec3("light[2].ambient", ambientColor);
            shaderProgram->setVec3("light[2].diffuse", diffuseColor);
            shaderProgram->setVec3("light[2].specular", 1.0f, 1.0f, 1.0f);


            lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
            diffuseColor = lightColor * glm::vec3(0.5f);
            ambientColor = diffuseColor * glm::vec3(0.2f);

            shaderProgram->setVec3("light[3].position", 0.0f,22.0f, 0.0f);
            shaderProgram->setVec3("light[3].ambient", ambientColor);
            shaderProgram->setVec3("light[3].diffuse", diffuseColor);
            shaderProgram->setVec3("light[3].specular", 1.0f, 1.0f, 1.0f);


            shaderProgram->setVec3("material.ambient", 0.3f, 0.6f, 0.2f);
            shaderProgram->setVec3("material.diffuse", 1.0f, 1.0f, 1.0f);
            shaderProgram->setVec3("material.specular", 0.5f, 0.5f, 0.5f);
            shaderProgram->setFloat("material.shininess", .5f);
     
        }

        if (modelType == Parallax)
        {
            shaderProgram->setFloat("heightScale", 0.01f);
        }


        if (texture0 && texture0->isLoaded) {
            shaderProgram->setInt("texture1", 0);
            texture0->bind(0);
        }

        if (texture1 && texture1->isLoaded) {
            shaderProgram->setInt("texture2", 1);
            texture1->bind(1);
        }

        if (texture2 && texture2->isLoaded) {
            shaderProgram->setInt("texture3", 2);
            texture2->bind(2);
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

    void renderAABB(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, std::shared_ptr<Shader> shaderProgram) const {
        // Get the transformed AABB
        AABB transformedAABB = getTransformedAABB();

        // Construct the model matrix
        glm::mat4 modelMatrix = glm::mat4(1.0f);

        glm::vec3 corners[8] = {
        transformedAABB.min,
        glm::vec3(transformedAABB.max.x, transformedAABB.min.y, transformedAABB.min.z),
        glm::vec3(transformedAABB.min.x, transformedAABB.max.y, transformedAABB.min.z),
        glm::vec3(transformedAABB.max.x, transformedAABB.max.y, transformedAABB.min.z),
        glm::vec3(transformedAABB.min.x, transformedAABB.min.y, transformedAABB.max.z),
        glm::vec3(transformedAABB.max.x, transformedAABB.min.y, transformedAABB.max.z),
        glm::vec3(transformedAABB.min.x, transformedAABB.max.y, transformedAABB.max.z),
        transformedAABB.max
        };

        // Define the indices for the wireframe lines
        GLuint indices[24] = {
            0, 1, 1, 3, 3, 2, 2, 0, // Bottom face
            4, 5, 5, 7, 7, 6, 6, 4, // Top face
            0, 4, 1, 5, 3, 7, 2, 6  // Vertical lines
        };

        // Define offsets for readability
        constexpr int POSITION_OFFSET = 0;      // x, y, z
        constexpr int TEXCOORD_OFFSET = 3;     // u, v
        constexpr int COLOR_OFFSET = 5;        // r, g, b
        constexpr int STRIDE = 8;              // Total attributes per vertex

        std::vector<float> lineVertices;

        // Fill vertex data
        for (int i = 0; i < 24; i++) {
            glm::vec3 corner = corners[indices[i]];
            lineVertices.push_back(corner.x);   // Position X
            lineVertices.push_back(corner.y);   // Position Y
            lineVertices.push_back(corner.z);   // Position Z
            lineVertices.push_back(0.0f);       // Texture Coord U (if unused, skip)
            lineVertices.push_back(0.0f);       // Texture Coord V (if unused, skip)
            lineVertices.push_back(0.0f);       // Color R
            lineVertices.push_back(1.0f);       // Color G
            lineVertices.push_back(0.0f);       // Color B
        }

        // Setup OpenGL buffers
        GLuint lineVAO, lineVBO;
        glGenVertexArrays(1, &lineVAO);
        glGenBuffers(1, &lineVBO);

        // Bind VAO
        glBindVertexArray(lineVAO);

        // Bind and fill VBO
        glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
        glBufferData(GL_ARRAY_BUFFER, lineVertices.size() * sizeof(float), lineVertices.data(), GL_STATIC_DRAW);

        // Set vertex attribute pointers
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, STRIDE * sizeof(float), (void*)(POSITION_OFFSET * sizeof(float)));
        glEnableVertexAttribArray(0); // Enable position attribute

        // If texture coordinates are unused, comment out the following two lines
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, STRIDE * sizeof(float), (void*)(TEXCOORD_OFFSET * sizeof(float)));
        glEnableVertexAttribArray(1); // Enable texture coord attribute

        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, STRIDE * sizeof(float), (void*)(COLOR_OFFSET * sizeof(float)));
        glEnableVertexAttribArray(2); // Enable color attribute

        // Unbind VAO to avoid accidental changes
        glBindVertexArray(0);

        // Optional: Unbind VBO (good practice, not strictly necessary)
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Activate the shader and set uniforms
        shaderProgram->use();
        shaderProgram->setMat4("projection", projectionMatrix);
        shaderProgram->setMat4("view", viewMatrix);
        shaderProgram->setMat4("transform", modelMatrix);

        // Render the AABB lines
        glBindVertexArray(lineVAO);
        glLineWidth(2.0f);
        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(lineVertices.size() / 6));
        glBindVertexArray(0);

        // Cleanup OpenGL buffers
        glDeleteVertexArrays(1, &lineVAO);
        glDeleteBuffers(1, &lineVBO);
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
            texture0 = std::make_shared<Texture>(path, GL_TEXTURE_2D);
        }
        else if (pos == 1) {
            texture1 = std::make_shared<Texture>(path, GL_TEXTURE_2D);
        }
        else if (pos == 2) {
            texture2 = std::make_shared<Texture>(path, GL_TEXTURE_2D);
        }
        else {
            std::cout << "Wrong texture pos" << std::endl;
        }

        if (texture0 && texture0->isLoaded && texture1 && texture1->isLoaded && texture2 && texture2->isLoaded) {
            modelType = Parallax;
        }
        else if (texture0 && texture0->isLoaded && texture1 && texture1->isLoaded ) {
            modelType = DoubleTextured;
        }
        else if ((texture0 && texture0->isLoaded) || (texture1 && texture1->isLoaded) ) {
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

    static bool checkCollision(const AABB& box1, const AABB& box2) {
        return (box1.min.x <= box2.max.x && box1.max.x >= box2.min.x) &&
            (box1.min.y <= box2.max.y && box1.max.y >= box2.min.y) &&
            (box1.min.z <= box2.max.z && box1.max.z >= box2.min.z);
    }
};
