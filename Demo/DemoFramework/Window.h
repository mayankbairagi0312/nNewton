#ifndef WINDOW_H
#define WINDOW_H

#include <SDL3/SDL.h>
#include<glad/gl.h>
#include <iostream>

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
	Window();
	~Window();
	bool Init();
	bool CreateWindow();
	void DestroyWindow();
	void Shutdown();
	SDL_Window* GetNativeHandle() const { return m_window; }
	bool IsValid() const; 
	bool IsInitialized() const;
	bool IsCreated() const;
	int getHeight()const { return m_windowHeight; }
	int getWidth()const { return m_windowWidth; }
	void HandleWindowEvent(const SDL_WindowEvent& windowEvent);
};
#endif 