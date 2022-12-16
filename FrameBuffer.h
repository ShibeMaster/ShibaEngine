#pragma once
#include <GL/glew.h>
#include <iostream>
#include "Display.h"
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
class FrameBuffer {
private:
	unsigned int fbo;
	unsigned int texture;
	unsigned int rbo;
public:
	glm::vec2 dimensions = glm::vec2(Display::width, Display::height);
	~FrameBuffer() {
		glDeleteFramebuffers(1, &fbo);
	}
	void Generate() {
		glGenFramebuffers(1, &fbo);
		Bind();
		glGenTextures(1, &texture);
		AttachTexture();
		glGenRenderbuffers(1, &rbo);
		GenerateRenderer();
		if (!CheckStatus())
			std::cout << "Framebuffer isn't complete" << std::endl;

		Unbind();
	}
	void GenerateRenderer() {
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, dimensions.x, dimensions.y);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	}
	void Clear() {
		Bind();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Unbind();
	}
	void Resize(glm::vec2 dimensions) {
		this->dimensions = dimensions;
		Bind();
		AttachTexture();
		GenerateRenderer();

		if (!CheckStatus())
			std::cout << "Framebuffer isn't complete" << std::endl;
		Unbind();
	}

	void AttachTexture() {
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, dimensions.x, dimensions.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	}
	unsigned int GetTexture() {
		return texture;
	}
	void Unbind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void Bind() {
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glEnable(GL_DEPTH_TEST);
	}

	static bool CheckStatus() {
		return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
	}
};