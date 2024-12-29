#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/gl.h>
#include "stb_image.h"
#include <string>
#include <iostream>

class Texture {
public:
    unsigned int ID;
    GLenum textureType;

    // Constructor
    Texture(const std::string& path, GLenum type, bool flip = true)
        : textureType(type)
    {
        if (flip) {
            stbi_set_flip_vertically_on_load(true);
        }

        glGenTextures(1, &ID);
        glBindTexture(type, ID);


        glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int width, height, nrChannels;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

        if (data) {
            GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
            glTexImage2D(type, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(type);
        }
        else {
            std::cerr << "Failed to load texture: " << path << std::endl;
        }

        stbi_image_free(data);
    }


    void bind(unsigned int slot = 0) const {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(textureType, ID);
    }

    void unbind() const {
        glBindTexture(textureType, 0);
    }

    void destroy() {
        glDeleteTextures(1, &ID);
    }

    ~Texture() {
        destroy();
    }
};

#endif
