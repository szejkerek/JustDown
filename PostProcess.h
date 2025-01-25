#pragma once

#include <glad/gl.h>
#include <vector>
#include <iostream>
#include <memory>
#include "Shader.h"

class PostProcess {
private:
    unsigned int hdrFBO, pingpongFBO[2];
    unsigned int colorBuffer, brightBuffer, pingpongColorbuffers[2];
    unsigned int rbo;
    unsigned int quadVAO = 0, quadVBO = 0;

    std::shared_ptr<Shader> brightShader;
    std::shared_ptr<Shader> blurShader;
    std::shared_ptr<Shader> combineShader;

    void checkFramebufferStatus() const {
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "Framebuffer is not complete! Status: " << status << std::endl;
        }
        else {
            std::cout << "Framebuffer is complete!" << std::endl;
        }
    }

    void checkOpenGLError() const {
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            std::cerr << "OpenGL error: " << err << std::endl;
        }
    }

public:
    void Init(int width, int height) {
        // Load shaders
        brightShader = std::make_shared<Shader>(
            "src/Shaders/PostProcess/Quad.vs",
            "src/Shaders/PostProcess/Brightness.fs");

        blurShader = std::make_shared<Shader>(
            "src/Shaders/PostProcess/Quad.vs",
            "src/Shaders/PostProcess/Blur.fs");

        combineShader = std::make_shared<Shader>(
            "src/Shaders/PostProcess/Quad.vs",
            "src/Shaders/PostProcess/Combine.fs");

        // HDR framebuffer
        glGenFramebuffers(1, &hdrFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

        // Color buffer
        glGenTextures(1, &colorBuffer);
        glBindTexture(GL_TEXTURE_2D, colorBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);

        // Bright buffer
        glGenTextures(1, &brightBuffer);
        glBindTexture(GL_TEXTURE_2D, brightBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, brightBuffer, 0);

        // Renderbuffer for depth and stencil
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

        // Set draw buffers
        unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        glDrawBuffers(2, attachments);
        checkFramebufferStatus();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Pingpong framebuffers
        glGenFramebuffers(2, pingpongFBO);
        glGenTextures(2, pingpongColorbuffers);
        for (unsigned int i = 0; i < 2; ++i) {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
            glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
            checkFramebufferStatus();
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Quad setup
        float quadVertices[] = {
            // Positions   // Texture Coords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f, -1.0f,  1.0f, 0.0f,

            -1.0f,  1.0f,  0.0f, 1.0f,
             1.0f, -1.0f,  0.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f
        };

        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);

        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        glBindVertexArray(0);

        checkOpenGLError(); // Check for OpenGL errors
    }

    void BeginRender() {
        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void ApplyBloom(float threshold, float bloomStrength) {
        // Brightness extraction
        brightShader->use();
        brightShader->setFloat("threshold", threshold);
        brightShader->setInt("inputTexture", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[0]);
        renderQuad();

        // Gaussian blur
        bool horizontal = true, firstIteration = true;
        unsigned int amount = 10;
        for (unsigned int i = 0; i < amount; ++i) {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
            blurShader->use();
            blurShader->setBool("horizontal", horizontal);
            blurShader->setInt("inputTexture", 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, firstIteration ? brightBuffer : pingpongColorbuffers[!horizontal]);
            renderQuad();
            horizontal = !horizontal;
            if (firstIteration)
                firstIteration = false;
        }

        // Combine pass
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        combineShader->use();
        combineShader->setFloat("bloomStrength", bloomStrength);
        combineShader->setInt("sceneTexture", 0);
        combineShader->setInt("bloomTexture", 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorBuffer);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
        renderQuad();
    }

    void renderQuad() const {
        if (quadVAO == 0) {
            std::cerr << "Quad VAO not initialized!" << std::endl;
            return;
        }
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

    ~PostProcess() {
        glDeleteFramebuffers(1, &hdrFBO);
        glDeleteTextures(1, &colorBuffer);
        glDeleteTextures(1, &brightBuffer);
        glDeleteRenderbuffers(1, &rbo);
        glDeleteFramebuffers(2, pingpongFBO);
        glDeleteTextures(2, pingpongColorbuffers);
        glDeleteVertexArrays(1, &quadVAO);
        glDeleteBuffers(1, &quadVBO);
    }
};
