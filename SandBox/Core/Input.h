#pragma once

#include <SDL3/SDL.h>
#include <glad/gl.h>
#include <imgui.h>
#include <string>
#include <unordered_map>
#include "Window.h"

class Camera;

class Input
{
public:
	// -- Constructors --
	explicit Input(Window* window) noexcept
		: m_window(window)
	{
	}
	Input(const Input&) = delete;
	Input& operator=(const Input&) = delete;
	Input(Input&&) noexcept = delete;
	Input& operator=(Input&&) noexcept = delete;
	~Input() = default;

	// -- Keyboard queries --
	bool IsKeyDown(SDL_Scancode scancode) const;
	bool IsKeyPressed(SDL_Scancode scancode) const;
	bool IsKeyReleased(SDL_Scancode scancode) const;

	// -- Mouse queries --
	bool IsMouseButtonDown(Uint8 button) const;
	bool IsMouseButtonPressed(Uint8 button) const;
	bool IsMouseButtonReleased(Uint8 button) const;

	void GetMousePosition(int* x, int* y) const;
	void GetMouseDelta(float* dx, float* dy) const;
	void GetMouseScroll(int* x, int* y) const;

	// -- Per-frame processing --
	void ProcessInputKey(float deltaTime);
	void ProcessMouseInput();
	void HandleWindowEvent(const SDL_WindowEvent& windowEvent);
	void BeginFrame();
	void ProcessEvent(const SDL_Event* event);
	void EndFrame();

	void SetCamera(Camera& camera) noexcept { m_camera = &camera; }

private:
	Window* m_window;
	std::unordered_map<SDL_Scancode, bool> m_keyboardState;
	std::unordered_map<SDL_Scancode, bool> m_previousKeyboardState;
	std::unordered_map<SDL_Scancode, bool> m_keyPressedThisFrame;
	std::unordered_map<SDL_Scancode, bool> m_keyReleasedThisFrame;

	struct MouseState {
		bool buttons[5] = { false };
		bool prevButtons[5] = { false };
		bool pressedButtons[5] = { false };
		bool releasedButtons[5] = { false };
		float x = 0.0f, y = 0.0f;
		float prevX = 0.0f, prevY = 0.0f;
		int scrollX = 0, scrollY = 0;
	} m_mouseState;

	Camera* m_camera = nullptr;
};
