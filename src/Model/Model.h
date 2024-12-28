#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include "glad/gl.h"
#include <GLFW/include/GLFW/glfw3.h>

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

class Model {
public:
    std::vector<Vertex> vertices;
    std::vector<TexCoord> texCoords;
    std::vector<Normal> normals;
    std::vector<Face> faces;
    GLuint VAO = 0, VBO = 0;

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
                std::cout << "Loaded vertex: (" << v.x << ", " << v.y << ", " << v.z << ")" << std::endl;
            }
            else if (prefix == "vt") {
                TexCoord tc;
                ss >> tc.u >> tc.v;
                texCoords.push_back(tc);
                std::cout << "Loaded texture coordinate: (" << tc.u << ", " << tc.v << ")" << std::endl;
            }
            else if (prefix == "vn") {
                Normal n;
                ss >> n.x >> n.y >> n.z;
                normals.push_back(n);
                std::cout << "Loaded normal: (" << n.x << ", " << n.y << ", " << n.z << ")" << std::endl;
            }
            else if (prefix == "f") {
                Face f = {};
                for (int i = 0; i < 3; ++i) {
                    std::string vertexData;
                    ss >> vertexData;

                    std::istringstream vertexStream(vertexData);
                    std::string index;

                    // Parse vertex index
                    if (std::getline(vertexStream, index, '/')) {
                        f.v[i] = std::stoi(index) - 1;
                    }

                    // Parse texture coordinate index
                    if (std::getline(vertexStream, index, '/')) {
                        f.t[i] = !index.empty() ? std::stoi(index) - 1 : -1;
                    }

                    // Parse normal index
                    if (std::getline(vertexStream, index)) {
                        f.n[i] = !index.empty() ? std::stoi(index) - 1 : -1;
                    }
                }
                faces.push_back(f);
                std::cout << "Loaded face: vertices (" << f.v[0] << ", " << f.v[1] << ", " << f.v[2] << ")";
                std::cout << ", texture coordinates (" << f.t[0] << ", " << f.t[1] << ", " << f.t[2] << ")";
                std::cout << ", normals (" << f.n[0] << ", " << f.n[1] << ", " << f.n[2] << ")" << std::endl;
            }
        }

        std::cout << "Finished loading model. Total vertices: " << vertices.size() << ", textures: " << texCoords.size() << ", normals: " << normals.size() << ", faces: " << faces.size() << std::endl;

        file.close();
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

        std::cout << "Total vertex data size: " << vertexData.size() << " floats." << std::endl;

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

        std::cout << "Buffers setup complete. VAO: " << VAO << ", VBO: " << VBO << std::endl;
    }

    void render() const {
        std::cout << "Rendering model..." << std::endl;
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(faces.size() * 3));
        glBindVertexArray(0);
        std::cout << "Rendering complete." << std::endl;
    }

    ~Model() {
        if (VAO) {
            std::cout << "Deleting VAO: " << VAO << std::endl;
            glDeleteVertexArrays(1, &VAO);
        }
        if (VBO) {
            std::cout << "Deleting VBO: " << VBO << std::endl;
            glDeleteBuffers(1, &VBO);
        }
    }
};
