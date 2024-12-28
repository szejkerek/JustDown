#ifndef SHADER_H
#define SHADER_H

#include <glad/gl.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <stdexcept>
#include <filesystem>
#include <glad/gl.h>
#include <glm/glm.hpp>

class Shader
{
public:
    unsigned int ID;

    Shader(const std::string& vertexPath, const std::string& fragmentPath)
    {
        std::string vertexCode = loadShaderSource(vertexPath);
        std::string fragmentCode = loadShaderSource(fragmentPath);

        unsigned int vertex = compileShader(vertexCode, GL_VERTEX_SHADER, "VERTEX");
        unsigned int fragment = compileShader(fragmentCode, GL_FRAGMENT_SHADER, "FRAGMENT");

        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");

        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    void use() const
    {
        glUseProgram(ID);
    }

    void setBool(const std::string& name, bool value) const
    {
        glUniform1i(getUniformLocation(name), static_cast<int>(value));
    }

    void setInt(const std::string& name, int value) const
    {
        glUniform1i(getUniformLocation(name), value);
    }

    void setFloat(const std::string& name, float value) const
    {
        glUniform1f(getUniformLocation(name), value);
    }
    void setVec2(const std::string& name, const glm::vec2& value) const
    {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec2(const std::string& name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }
    // ------------------------------------------------------------------------
    void setVec3(const std::string& name, const glm::vec3& value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec3(const std::string& name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
    // ------------------------------------------------------------------------
    void setVec4(const std::string& name, const glm::vec4& value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec4(const std::string& name, float x, float y, float z, float w) const
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }
    // ------------------------------------------------------------------------
    void setMat2(const std::string& name, const glm::mat2& mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat3(const std::string& name, const glm::mat3& mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string& name, const glm::mat4& mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }


    ~Shader()
    {
        glDeleteProgram(ID);
    }

private:
    mutable std::unordered_map<std::string, int> uniformLocationCache;

    static std::string loadShaderSource(const std::string& filePath)
    {
        if (!std::filesystem::exists(filePath))
        {
            throw std::runtime_error("Shader file not found: " + filePath);
        }

        std::ifstream shaderFile(filePath);
        std::stringstream shaderStream;
        shaderStream << shaderFile.rdbuf();
        return shaderStream.str();
    }

    static unsigned int compileShader(const std::string& source, GLenum type, const std::string& shaderTypeName)
    {
        unsigned int shader = glCreateShader(type);
        const char* sourceCStr = source.c_str();
        glShaderSource(shader, 1, &sourceCStr, nullptr);
        glCompileShader(shader);

        int success;
        char infoLog[1024];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            throw std::runtime_error("ERROR::SHADER_COMPILATION_ERROR of type: " + shaderTypeName + "\n" + infoLog);
        }

        return shader;
    }

    void checkCompileErrors(unsigned int object, const std::string& type) const
    {
        int success;
        char infoLog[1024];
        if (type == "PROGRAM")
        {
            glGetProgramiv(object, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(object, 1024, nullptr, infoLog);
                throw std::runtime_error("ERROR::PROGRAM_LINKING_ERROR\n" + std::string(infoLog));
            }
        }
    }

    int getUniformLocation(const std::string& name) const
    {
        if (uniformLocationCache.find(name) != uniformLocationCache.end())
        {
            return uniformLocationCache[name];
        }

        int location = glGetUniformLocation(ID, name.c_str());
        if (location == -1)
        {
            std::cerr << "WARNING::UNIFORM " << name << " NOT FOUND" << std::endl;
        }
        uniformLocationCache[name] = location;
        return location;
    }
};

#endif
