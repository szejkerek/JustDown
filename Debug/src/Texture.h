#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/gl.h>
#include <string>
#include <iostream>
#include "stb_image.h"

class Texture {
public:
    unsigned int ID;
    GLenum textureType;
    bool isLoaded = false; // Flaga okreœlaj¹ca, czy tekstura zosta³a za³adowana

    Texture() = default;

    // Konstruktor
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
            isLoaded = true; // Ustawienie flagi na true, gdy tekstura zosta³a poprawnie za³adowana
        }
        else {
            std::cerr << "Failed to load texture: " << path << std::endl;
            isLoaded = false;
        }

        stbi_image_free(data);
    }

    void bind(unsigned int slot = 0) const {
        if (isLoaded) {
            glActiveTexture(GL_TEXTURE0 + slot);
            glBindTexture(textureType, ID);
        }
    }

    void unbind() const {
        if (isLoaded) {
            glBindTexture(textureType, 0);
        }
    }

    void destroy() {
        if (isLoaded) {
            glDeleteTextures(1, &ID);
            isLoaded = false;
        }
    }

    ~Texture() {
        destroy();
    }
};


#endif
