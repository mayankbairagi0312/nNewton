#pragma once
#include <GLAD/gl.h>
#include "Core/Window.h"

class SandboxFramebuffer
{
public:
	void Initialize(const int Width , const int Height)
	{
		m_Width = Width;
		m_Height = Height;
		glGenFramebuffers(1, &m_FrameBuff);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuff);

		glGenTextures(1, &m_FrameTexture);
		glBindTexture(GL_TEXTURE_2D, m_FrameTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_FrameTexture, 0);


		glGenRenderbuffers(1, &m_RenderBuff);
		glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBuff);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Width, Height);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderBuff);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cerr << "[FBO] INCOMPLETE! " << Width << "x" << Height << "\n";

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Bind()
	{

		glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuff);
		glViewport(0, 0,m_Width, m_Height);
	}
	void Resize(int w , int h)
	{
		if (w == m_Width && h == m_Height) return;
		if (w <= 0 || h <= 0) return;
		Destory();
		Initialize(w, h);
	}

	void Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Destory()
	{
		glDeleteTextures(1, &m_FrameTexture);
		glDeleteRenderbuffers(1, &m_RenderBuff);
		glDeleteFramebuffers(1, &m_FrameBuff);
	}
	int Height()const
	{
		return m_Height;
	}
	int Width()const
	{
		return m_Width;
	}

	unsigned int getFrameTexture() const
	{
		return m_FrameTexture;
	}

	static void ClearFrameBuff() {
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

private:
	unsigned int m_FrameBuff;
	unsigned int m_FrameTexture;
	unsigned int m_RenderBuff;
	int m_Height;
	int m_Width;
};