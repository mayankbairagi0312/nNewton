#include "Input.h"
#include "Renderer/Camera.hpp"

//====================== Keyboard =======================//

bool Input::IsKeyDown(SDL_Scancode scancode) const
{
	auto it = m_keyboardState.find(scancode);
	return (it != m_keyboardState.end() && it->second);
}

bool Input::IsKeyPressed(SDL_Scancode scancode) const
{
	auto it = m_keyPressedThisFrame.find(scancode);
	return (it != m_keyPressedThisFrame.end() && it->second);
}

bool Input::IsKeyReleased(SDL_Scancode scancode) const
{
	auto it = m_keyReleasedThisFrame.find(scancode);
	return (it != m_keyReleasedThisFrame.end() && it->second);
}

//====================== Mouse =======================//

bool Input::IsMouseButtonDown(Uint8 button) const
{
	const int index = button - 1;
	if (index >= 0 && index < 5) return m_mouseState.buttons[index];
	return false;
}

bool Input::IsMouseButtonPressed(Uint8 button) const
{
	const int index = button - 1;
	if (index >= 0 && index < 5) return m_mouseState.pressedButtons[index];
	return false;
}

bool Input::IsMouseButtonReleased(Uint8 button) const
{
	const int index = button - 1;
	if (index >= 0 && index < 5) return m_mouseState.releasedButtons[index];
	return false;
}

void Input::GetMousePosition(int* x, int* y) const
{
	if (x) *x = static_cast<int>(m_mouseState.x);
	if (y) *y = static_cast<int>(m_mouseState.y);
}

void Input::GetMouseDelta(float* dx, float* dy) const
{
	if (dx) *dx = m_mouseState.x - m_mouseState.prevX;
	if (dy) *dy = m_mouseState.y - m_mouseState.prevY;
}

void Input::GetMouseScroll(int* x, int* y) const
{
	if (x) *x = m_mouseState.scrollX;
	if (y) *y = m_mouseState.scrollY;
}

//====================== Frame processing =======================//

void Input::BeginFrame()
{
	// Store previous state.
	m_previousKeyboardState = m_keyboardState;
	for (int i = 0; i < 5; ++i) {
		m_mouseState.prevButtons[i] = m_mouseState.buttons[i];
	}

	m_mouseState.prevX = m_mouseState.x;
	m_mouseState.prevY = m_mouseState.y;

	// Clear per-frame state.
	m_keyPressedThisFrame.clear();
	m_keyReleasedThisFrame.clear();

	for (int i = 0; i < 5; ++i)
	{
		m_mouseState.pressedButtons[i] = false;
		m_mouseState.releasedButtons[i] = false;
	}

	m_mouseState.scrollX = 0;
	m_mouseState.scrollY = 0;
}

void Input::ProcessEvent(const SDL_Event* event)
{
	switch (event->type)
	{
	case SDL_EVENT_KEY_DOWN:
	{
		const SDL_Scancode scancode = event->key.scancode;
		m_keyboardState[scancode] = true;
		if (!event->key.repeat) { m_keyPressedThisFrame[scancode] = true; }
		break;
	}
	case SDL_EVENT_KEY_UP:
	{
		const SDL_Scancode scancode = event->key.scancode;
		m_keyboardState[scancode] = false;
		m_keyReleasedThisFrame[scancode] = true;
		break;
	}
	case SDL_EVENT_MOUSE_BUTTON_DOWN:
	{
		const Uint8 button = event->button.button;
		const int index = button - 1;
		if (index >= 0 && index < 5)
		{
			m_mouseState.buttons[index] = true;
			m_mouseState.pressedButtons[index] = true;
		}
		SDL_CaptureMouse(true);
		break;
	}
	case SDL_EVENT_MOUSE_BUTTON_UP:
	{
		const Uint8 button = event->button.button;
		const int index = button - 1;
		if (index >= 0 && index < 5)
		{
			m_mouseState.buttons[index] = false;
			m_mouseState.releasedButtons[index] = true;
		}
		SDL_CaptureMouse(true);
		break;
	}
	case SDL_EVENT_MOUSE_MOTION:
	{
		m_mouseState.x = event->motion.x;
		m_mouseState.y = event->motion.y;
		break;
	}
	case SDL_EVENT_MOUSE_WHEEL:
	{
		const ImGuiIO& io = ImGui::GetIO();
		if (!io.WantCaptureMouse) {
			m_mouseState.scrollX += static_cast<int>(event->wheel.x);
			m_mouseState.scrollY += static_cast<int>(event->wheel.y);
			m_camera->ProcessMouseScroll(m_mouseState.scrollX, m_mouseState.scrollY);
		}
		break;
	}
	case SDL_EVENT_WINDOW_RESIZED:
	case SDL_EVENT_WINDOW_MINIMIZED:
	case SDL_EVENT_WINDOW_RESTORED:
		HandleWindowEvent(event->window);
		break;
	}
}

void Input::ProcessInputKey(float deltaTime)
{
	if (IsKeyDown(SDL_SCANCODE_W)) {
		m_camera->ProcessKeyboard(m_camera->GetFront(), deltaTime);
	}
	if (IsKeyDown(SDL_SCANCODE_S)) {
		m_camera->ProcessKeyboard(-m_camera->GetFront(), deltaTime);
	}
	if (IsKeyDown(SDL_SCANCODE_A)) {
		m_camera->ProcessKeyboard(-m_camera->GetRight(), deltaTime);
	}
	if (IsKeyDown(SDL_SCANCODE_D)) {
		m_camera->ProcessKeyboard(m_camera->GetRight(), deltaTime);
	}
}

void Input::ProcessMouseInput()
{
	float xoffset = 0.0f;
	float yoffset = 0.0f;
	GetMouseDelta(&xoffset, &yoffset);

	if (IsMouseButtonDown(1))
	{
		m_camera->ProcessMouseMove(xoffset, yoffset, true);
	}

	if (IsMouseButtonDown(2))
	{
		m_camera->ProcessMousePan(xoffset, yoffset);
	}

	m_mouseState.prevX = m_mouseState.x;
	m_mouseState.prevY = m_mouseState.y;
}

void Input::HandleWindowEvent(const SDL_WindowEvent& windowEvent)
{
	switch (windowEvent.type) {

	case SDL_EVENT_WINDOW_RESIZED:
		m_window->SetWindow(windowEvent.data1, windowEvent.data2);
		break;

	case SDL_EVENT_WINDOW_MINIMIZED:
	case SDL_EVENT_WINDOW_RESTORED:
		break;
	}
}

void Input::EndFrame()
{
	for (int i = 0; i < 5; ++i)
	{
		m_mouseState.pressedButtons[i] = false;
		m_mouseState.releasedButtons[i] = false;
	}
	m_mouseState.scrollX = 0;
	m_mouseState.scrollY = 0;
}
