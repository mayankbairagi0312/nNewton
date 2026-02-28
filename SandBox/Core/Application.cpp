#include "Application.h"

Application::Application()
	: m_testWindow(nullptr), m_running(false), m_CurrTime(0), m_PrevTime(SDL_GetPerformanceCounter()), m_DeltaTime(0.0f) {
}

bool Application::Initialize() {

	std::cout << ">>>===================================================================<<<\n >>       nNEWTON TEST BUILD       <<\n>>>===================================================================<<<\n>>        VERSION 0.1.0       <<\n"
		<< ">>>===================================================================<<<\n" << std::endl;


	m_testWindow = std::make_unique<Window>();

	m_Debug_Renderer = std::make_shared<DebugRenderer>();
	//m_debugDrawer = new OpneGLDebugRenderer();
   /* m_debugDrawer = std::make_unique<OpneGLDebugRenderer>();
	m_TestDebugRenderer.SetDrawer(m_debugDrawer.get());*/

	// Initialize window 
	if (!m_testWindow->Init()) {
		std::cerr << "Failed to initialize window system!" << std::endl;
		return false;
	}

	//  window init
	if (!m_testWindow->CreateWindow()) {
		std::cerr << "Failed to create window!" << std::endl;
		return false;
	}

	glViewport(0, 0, m_testWindow->getWidth(), m_testWindow->getHeight());

	// init UI
	if (!m_DebugUI->Init_DebugUIEditor(m_testWindow.get(), m_Debug_Renderer))
	{
		std::cerr << "UI : kya cheda bosdi \n" << std::endl;
	}

	// cam for input
	m_input.SetCamera(m_camera);


	//init debug render
	if (!m_Render_System.INIT_DEBUG_RENDER(&m_camera, m_Debug_Renderer))
	{
		std::cerr << "camera ki MKC" << std::endl;
	}

	//default scene setup
	m_Render_System.defaultScene();



	m_camera.setPosition(nNewton::nVector3(0.0f, 5.0f, 15.0f));
	//m_camera.setProjection(Camera::ProjectionType::Orthographic,100,(float)(m_testWindow->getWidth()/ m_testWindow->getHeight()),0,150.5);
	m_running = true;

	std::cout << "Application initialized successfully.\n";

	return true;

}

void Application::Run() {

	if (!m_testWindow || !m_testWindow->IsValid()) {
		std::cerr << "Cannot run: Window is not valid!" << std::endl;
		return;
	}

	m_PrevTime = SDL_GetPerformanceCounter();
	std::cout << "Application starting main loop...\n";
	while (m_running)
	{

		m_CurrTime = SDL_GetPerformanceCounter();
		m_DeltaTime = (double)((m_CurrTime - m_PrevTime) / (double)SDL_GetPerformanceFrequency());
		m_PrevTime = m_CurrTime;



		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			ImGui_ImplSDL3_ProcessEvent(&event);
			if (event.type == SDL_EVENT_QUIT) {
				m_running = false;
			}
			m_input.ProcessEvent(&event);
		}
		m_input.BeginFrame();
		m_DebugUI->BeginUIFrame();



		bool pauseGameInput = ImGui::GetIO().WantCaptureMouse;

		static bool demo = true;
		//ImGui::ShowDemoWindow(&demo);
		m_DebugUI->Stats_Overlay(&demo);

		m_input.ProcessInputKey(m_DeltaTime);
		if (!pauseGameInput) {
			m_input.ProcessMosueInput();
		}

		TRender();


		m_DebugUI->EndUIFrame();
		SDL_GL_SwapWindow(m_testWindow->GetNativeHandle());
		m_input.EndFrame();
	}
}



void Application::TRender()
{
	m_Render_System.Start_Debug_Draw();
	m_Render_System.Debug_DrawAxis(nNewton::nVector3(m_camera.GetPosition().x, m_camera.GetPosition().y, m_camera.GetPosition().z));

	m_Debug_Renderer->SetFlagEnabled(flags::Shapes);

	m_Render_System.Debug_Render();

	m_Render_System.End_Debug_Draw();
}

void Application::Shutdown() {
	std::cout << "Application shutting down\n";
	std::cout << "data khatam, khel khatam. beta!!!!\n";
	m_Render_System.ShutDown_DebugRender();
	if (m_testWindow) {

		m_testWindow->Shutdown();
		m_DebugUI->ShutDownUI();
	}

	m_testWindow.reset();
	m_running = false;
}

Application::~Application() {
	//Shutdown();  

}