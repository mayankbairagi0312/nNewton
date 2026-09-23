#pragma once

#include <SDL3/SDL.h>
#include <glad/gl.h>
#include <stdexcept>

class Window
{
private:
	SDL_Window* m_window;
	SDL_GLContext m_glContext;

	// Window state
	int m_windowWidth;
	int m_windowHeight;
	bool m_isFullscreen;

	bool m_isInitialized;
	bool m_isCreated;

public:
	// -- Constructors --
	Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	Window(Window&&) noexcept = delete;
	Window& operator=(Window&&) noexcept = delete;
	~Window();

	// -- Lifecycle --
	bool Init();
	bool CreateWindow();
	void DestroyWindow();
	void Shutdown();

	// -- Queries --
	SDL_Window* GetNativeHandle() const { return m_window; }
	SDL_GLContext GetGLContext() const { return m_glContext; }
	bool IsValid() const noexcept { return m_window != nullptr; }
	bool IsInitialized() const { return m_isInitialized; }
	bool IsCreated() const { return m_isCreated; }
	int GetHeight() const noexcept { return m_windowHeight; }
	int GetWidth() const noexcept { return m_windowWidth; }

	// -- Resize --
	void SetWindow(int width, int height);
};
