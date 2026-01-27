#pragma once

#include <unordered_map>
#include <string>
#include<SDL3/SDL.h>
#include "Window.h"
#include "Camera.hpp"
class Input
{
public:
	bool IsKeyDown(SDL_Scancode scancode) const;
	bool IsKeyPressed(SDL_Scancode scancode) const;
	bool IsKeyReleased(SDL_Scancode scancode)const;

	bool IsMouseButtonDown(Uint8 button)const;
	bool IsMouseButtonPressed(Uint8 button)const;
	bool IsMouseButtonRelesed(Uint8 button)const;

	void GetMousePosition(int* x, int* y)const;
	void GetMouseDelta(float* dx, float* dy)const;
	void GetMouseScroll(int* x, int* y)const;
	void ProcessInputKey(float deltaTime);
	void ProcessMosueInput();
	void BeginFrame();
	void ProcessEvent(const SDL_Event* event);
	void EndFrame();
	void SetCamera(Camera& camera);
private:
	Window testwindow;
	std::unordered_map<SDL_Scancode, bool> m_keyboardState;
	std::unordered_map<SDL_Scancode, bool> m_previousKeyboardState;
	std::unordered_map<SDL_Scancode, bool> m_keyPressedThisFrame;
	std::unordered_map<SDL_Scancode, bool> m_keyReleasedThisFrame;

	struct MouseState {
		bool buttons[5] = { false }; 
		bool prevButtons[5] = { false };
		bool pressedButtons[5] = { false };
		bool releasedButtons[5] = { false };
		float x = 0, y = 0;
		float prevX = 0, prevY = 0;
		int scrollX = 0, scrollY = 0;
	} m_mouseState;
	Camera* m_camera;
	
};