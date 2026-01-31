#include <imgui.h>
#include<imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>
#include "Core/Window.h"
#include"Renderer/DebugRenderer.hpp"
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

	void Stats_Overlay(bool* IsOverlay ,  DebugRenderer* debugRenderer)
	{
		static int location = 0;
		ImGuiIO& io = ImGui::GetIO();
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
		if (location >= 0)
		{
			const float	PAD = 10.0f;
			const ImGuiViewport* viewport = ImGui::GetMainViewport();

			ImVec2 work_pos = viewport->WorkPos;
			ImVec2 work_size = viewport->WorkSize;
			ImVec2 Window_POS, Window_POS_Pivot;

			Window_POS.x = (location & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
			Window_POS.y = (location & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
			Window_POS_Pivot.x = (location & 1) ? 1.0f : 0.0f;
			Window_POS_Pivot.y = (location & 2) ? 1.0f : 0.0f;
			ImGui::SetNextWindowPos(Window_POS, ImGuiCond_Always, Window_POS_Pivot);

			//UI WINDOW SIZE 
			ImGui::SetNextWindowSize(ImVec2(450.0f, 350.0f),ImGuiCond_Always);
			window_flags |= ImGuiWindowFlags_NoMove;

		}

		
		ImGui::SetNextWindowBgAlpha(0.2f);
		ImGui::Begin("Stats Overlay", IsOverlay, window_flags);
		//ImGui::SetWindowFontScale(1.5f);
		io.FontGlobalScale = 2.0f;
		ImGui::Text("nNewton Statistics:\n""Mayank Bairagi");
		ImGui::Separator();
		
		ImGui::Text("FrameRate : %.1f FPS\nFrameTime : %.1f MS", io.Framerate, 1000/io.Framerate);

		ImGui::Separator();
		
		if (ImGui::IsMousePosValid())
			ImGui::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
		else
			ImGui::Text("Mouse Position: <invalid>");

		ImGui::Separator();

		ImGui::Text("Numbers of Objects: %s\n","NA");
		int lineCount = debugRenderer->GetLineCount() * 2;
		//printf("Stats_Overlay lineCount = %d\n", lineCount);
		ImGui::Text("Numbers of Vertex: %d\n",lineCount);

		ImGui::Separator();

		ImGui::Text("Numbers of Collision: %s\n","NA");

		ImGui::Separator();
		int w, h;
		SDL_GetWindowSize(SDL_Window->GetNativeHandle(),&w,&h );
		ImGui::Text("Window Size: %d X %d\n",w,h);
		ImGui::End();

	}
	
};