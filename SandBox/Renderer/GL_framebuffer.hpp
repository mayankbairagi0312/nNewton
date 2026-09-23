#pragma once
#include <GLAD/gl.h>
#include <iostream>
#include "Core/Window.h"

class SandboxFramebuffer
{
public:
	// -- Constructors --
	SandboxFramebuffer() = default;
	SandboxFramebuffer(const SandboxFramebuffer&) = delete;
	SandboxFramebuffer& operator=(const SandboxFramebuffer&) = delete;
	SandboxFramebuffer(SandboxFramebuffer&&) noexcept = delete;
	SandboxFramebuffer& operator=(SandboxFramebuffer&&) noexcept = delete;
	~SandboxFramebuffer() = default;

	// -- Lifecycle --
	void Initialize(int width, int height)
	{
		m_Width = width;
		m_Height = height;
		glGenFramebuffers(1, &m_FrameBuff);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuff);

		glGenTextures(1, &m_FrameTexture);
		glBindTexture(GL_TEXTURE_2D, m_FrameTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_FrameTexture, 0);

		glGenRenderbuffers(1, &m_RenderBuff);
		glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBuff);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderBuff);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cerr << "[FBO] INCOMPLETE! " << width << "x" << height << "\n";

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuff);
		glViewport(0, 0, m_Width, m_Height);
	}

	void Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Resize(int width, int height)
	{
		if (width == m_Width && height == m_Height) return;
		if (width <= 0 || height <= 0) return;
		Destroy();
		Initialize(width, height);
	}

	void Destroy()
	{
		glDeleteTextures(1, &m_FrameTexture);
		glDeleteRenderbuffers(1, &m_RenderBuff);
		glDeleteFramebuffers(1, &m_FrameBuff);
	}

	// -- Queries --
	int GetHeight() const noexcept { return m_Height; }
	int GetWidth() const noexcept { return m_Width; }
	unsigned int GetFrameTexture() const noexcept { return m_FrameTexture; }

	// -- Static helpers --
	static void ClearFrameBuff()
	{
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

private:
	unsigned int m_FrameBuff = 0;
	unsigned int m_FrameTexture = 0;
	unsigned int m_RenderBuff = 0;
	int m_Height = 0;
	int m_Width = 0;
};
