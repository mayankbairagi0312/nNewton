#include <imgui.h>
#include<imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>
#include "Core/Window.h"
#include <memory>

class DebugUIEditor {

private:
	Window* SDL_Window; 
public:
	bool Init_DebugUIEditor(Window* window)
	{	
		SDL_Window = window;

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		
		//io.DisplaySize = ImVec2((float)SDL_Window->getWidth(), (float)SDL_Window->getHeight());

		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		

		ImGui_ImplSDL3_InitForOpenGL(SDL_Window->GetNativeHandle(), SDL_Window->GetSDLglContext() );
		ImGui_ImplOpenGL3_Init("#version 460");

		return true;
	}

	void BeginUIFrame()
	{	
		
		ImGui_ImplOpenGL3_NewFrame();

		ImGui_ImplSDL3_NewFrame();

		ImGui::NewFrame();
		
	}
	void EndUIFrame()
	{
		ImGui::Render();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	}

	void ShutDownUI()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL3_Shutdown();
		ImGui::DestroyContext();
	}

	
};