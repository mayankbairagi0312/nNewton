#include "Input.h"

bool Input::IsKeyDown(SDL_Scancode scancode)const
{
	auto it = m_keyboardState.find(scancode);
	return (it != m_keyboardState.end() && it->second);
}

bool Input::IsKeyPressed(SDL_Scancode scancode)const
{
	auto it = m_keyPressedThisFrame.find(scancode);
	return (it != m_keyPressedThisFrame.end() && it->second);
}

bool Input::IsKeyReleased(SDL_Scancode scancode)const
{
	auto it = m_keyReleasedThisFrame.find(scancode);
	return (it != m_keyReleasedThisFrame.end() && it->second);
}

bool Input::IsMouseButtonDown(Uint8 button)const
{
	int index = button - 1;
	if (index >= 0 && index < 5)return m_mouseState.buttons[index];
	return false;
}

bool Input::IsMouseButtonPressed(Uint8 button)const
{
	int index = button - 1;
	if (index >= 0 && index < 5)return m_mouseState.pressedButtons[index];
	return false;
}

bool Input::IsMouseButtonRelesed(Uint8 button)const
{
	int index = button - 1;
	if (index >= 0 && index < 5)return m_mouseState.releasedButtons[index];
	return false;
}

void Input::GetMousePosition(int* x, int* y)const
{
	if (x)*x = m_mouseState.x;
	if (y)*y = m_mouseState.y;
}

void Input::GetMouseDelta(float* dx, float* dy)const
{
	if (dx)*dx = m_mouseState.x - m_mouseState.prevX;
	if (dy)*dy = m_mouseState.y - m_mouseState.prevY;
}

void Input::GetMouseScroll(int* x, int* y)const
{
	if (x)*x = m_mouseState.scrollX;
	if (y)*y = m_mouseState.scrollY;
}

void Input::BeginFrame() {

	//store prev state
	m_previousKeyboardState = m_keyboardState;
	for (int i = 0; i < 5; ++i) {
		m_mouseState.prevButtons[i] = m_mouseState.buttons[i];
	}
	

	// clear frame 
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
		SDL_Scancode sc = event->key.scancode;

		m_keyboardState[sc] = true;

		if (!event->key.repeat) { m_keyPressedThisFrame[sc] = true; }
		break;
	}
	case SDL_EVENT_KEY_UP: 
	{
		SDL_Scancode sc = event->key.scancode;

		m_keyboardState[sc] = false;
		m_keyReleasedThisFrame[sc] = true;
		break;
	}
	case SDL_EVENT_MOUSE_BUTTON_DOWN: 
	{	
		Uint8 button = event->button.button;
		int index = button - 1;
		if (index >= 0 && index < 5)
		{
			m_mouseState.buttons[index] = true;
			m_mouseState.pressedButtons[index] = true;
		}

		if (ImGui::GetIO().WantCaptureMouse) {
			// Optional: Reset your game-specific "pressed" flags so the camera doesn't jump
			//m_mouseState.pressedButtons[index] = false;
		}
		else {
			SDL_CaptureMouse(true); // Only capture for the game if ImGui doesn't want it
		}
	
		break;
	}
	case SDL_EVENT_MOUSE_BUTTON_UP:
	{
		Uint8 button = event->button.button;
		int index = button - 1;
		if (index >= 0 && index < 5)
		{
			m_mouseState.buttons[index] = false;
			m_mouseState.releasedButtons[index] = true;
		}
		SDL_CaptureMouse(true);
		break;
	}
	case SDL_EVENT_MOUSE_MOTION: {
		m_mouseState.x = static_cast<int>(event->motion.x);
		m_mouseState.y = static_cast<int>(event->motion.y);
		break;
	}
	case SDL_EVENT_MOUSE_WHEEL: {
		m_mouseState.scrollX += static_cast<int>(event->wheel.x);
		m_mouseState.scrollY += static_cast<int>(event->wheel.y);
		m_camera->ProcessMouseScroll(m_mouseState.scrollX , m_mouseState.scrollY);
		break;
	}
	case SDL_EVENT_WINDOW_RESIZED:
		testwindow.HandleWindowEvent(event->window);
		break;
	case SDL_EVENT_WINDOW_MINIMIZED:
	case SDL_EVENT_WINDOW_RESTORED:
		testwindow.HandleWindowEvent(event->window);
		break;
	}
}

void Input::ProcessInputKey(float deltaTime)
{
	
    if (IsKeyDown(SDL_SCANCODE_W)) {
        printf("W pressed!\n");
        m_camera->ProcessKeyboard(m_camera->GetFront(), deltaTime);
    }
    if (IsKeyDown(SDL_SCANCODE_S)) {
        printf("S pressed!\n");
        m_camera->ProcessKeyboard(-m_camera->GetFront(), deltaTime);  
    }
    if (IsKeyDown(SDL_SCANCODE_A)) {
        printf("A pressed!\n");
        m_camera->ProcessKeyboard(-m_camera->GetRight(), deltaTime);  
    }
    if (IsKeyDown(SDL_SCANCODE_D)) {
        printf("D pressed!\n");
        m_camera->ProcessKeyboard(m_camera->GetRight(), deltaTime);  
    }
	
}
void Input::ProcessMosueInput()
{	
	if (ImGui::GetIO().WantCaptureMouse)
		return;

	float xoffset, yoffset;
	GetMouseDelta(&xoffset, &yoffset);

	if (IsMouseButtonDown(1))
	{

		m_camera->ProcessMouseMove(xoffset, yoffset, true);
	}
	

	if (IsMouseButtonDown(2))
	{
		m_camera->ProcessMousePan(xoffset, yoffset);
		printf("===> Mid Mouse Down <===\n");
	}
	m_mouseState.prevX = m_mouseState.x;
	m_mouseState.prevY = m_mouseState.y;

}


void Input::EndFrame() {
	for (int i = 0; i < 5; ++i)
	{
		m_mouseState.pressedButtons[i] = false;
		m_mouseState.releasedButtons[i] = false;
	}
	m_mouseState.scrollX = 0;
	m_mouseState.scrollY = 0;
}

void Input::SetCamera(Camera& camera)
{	
	m_camera = &camera;
}

