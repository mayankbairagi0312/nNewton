#include "Application.h"
#include<chrono>
Application::Application()
	: m_testWindow(nullptr), m_running(false), m_CurrTime(0), m_PrevTime(SDL_GetPerformanceCounter()), m_DeltaTime(0.0f) {
}

bool Application::Initialize() {

	std::cout << ">>>===================================================================<<<\n >>       nNEWTON TEST BUILD       <<\n>>>===================================================================<<<\n>>        VERSION 0.1.0       <<\n"
		<< ">>>===================================================================<<<\n" << std::endl;
	

	m_testWindow = std::make_unique<Window>();

	m_Debug_Renderer = std::make_shared<DebugRenderer>();
	
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
	if (!m_DebugUI->Init_DebugUIEditor(m_testWindow.get(), m_Debug_Renderer, m_nWorld.get(), &m_Render_System,m_FrameBuffer.get())){
		std::cerr << "===> UI : Failed to init UI  <===\n" << std::endl;
	}

	m_input.SetCamera(m_camera);


	//init physics sys
	if (!m_physicsSystem->INIT_PHYSICS_SYS(m_nWorld.get())){
		std::cerr << "===> Physics : Failed to init <===" << std::endl;
	};

	//init debug render
	if (!m_Render_System.INIT_DEBUG_RENDER(&m_camera, m_Debug_Renderer, m_nWorld->GetCollisionWorld(), m_nWorld.get())){
		std::cerr << "CAM! CAM! CAM! FAILED INIT " << std::endl;
	}

	//build Tree 
	m_nWorld->GetCollisionWorld()->BuildTrees();
	//m_Render_System.Debug_DrawAxis(nNewton::nVector3(m_camera.GetPosition().x, m_camera.GetPosition().y, m_camera.GetPosition().z));

	m_running = true;
	std::cout << "=== > Application initialized successfully < ===.\n";
	DebugUIEditor::AddLog("===> Application initialized Successfully <===");
	DebugUIEditor::AddLog("===>        Welcome to nNewton            <===");
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
		m_DeltaTime = static_cast<float>(
			(m_CurrTime - m_PrevTime) / static_cast<double>(SDL_GetPerformanceFrequency())
			);
		m_PrevTime = m_CurrTime;

		m_input.BeginFrame();

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			ImGui_ImplSDL3_ProcessEvent(&event);
			if (event.type == SDL_EVENT_QUIT) {
				m_running = false;
			}
			m_input.ProcessEvent(&event);
		}

		SandboxFramebuffer::ClearFrameBuff();
		m_physicsSystem->UpdatePhysicsSystem(m_DeltaTime);
		
		
		//======================
		m_DebugUI->BeginUIFrame();
		m_DebugUI->ViewportBegin(&m_camera);

		TRender();
		static bool demo = true;
		m_DebugUI->ViewportEnd(&demo);
	

		if(m_DebugUI->IsProcessMouse()) m_input.ProcessMosueInput();
		if (m_DebugUI->IsViewportFocused() && m_DebugUI->IsViewportHovered()) {
			m_input.ProcessInputKey(m_DeltaTime);
		}


		ImGui::ShowDemoWindow(&demo);
		m_DebugUI->RenderUI( &demo);
		
        m_DebugUI->EndUIFrame();
		//======================
		
		SDL_GL_SwapWindow(m_testWindow->GetNativeHandle());
		m_input.EndFrame();
	}
}



void Application::TRender()
{
	auto t0 = std::chrono::high_resolution_clock::now();
	m_FrameBuffer->Bind();
	auto t1 = std::chrono::high_resolution_clock::now();
	m_Render_System.Start_Debug_Draw();
	auto t2 = std::chrono::high_resolution_clock::now();

	m_Debug_Renderer->SetFlagEnabled(flags::Shapes);
	m_Render_System.Debug_DrawAxis(m_camera.GetPosition());
	m_Render_System.Debug_Render();
	auto t3 = std::chrono::high_resolution_clock::now();
	m_Render_System.End_Debug_Draw();
	auto t4 = std::chrono::high_resolution_clock::now();
	m_FrameBuffer->Unbind();
	auto t5 = std::chrono::high_resolution_clock::now();

	/*printf(
		"Bind: %.3f ms | Start: %.3f ms | DebugRender: %.3f ms | End: %.3f ms | Unbind: %.3f ms\n",
		std::chrono::duration<double, std::milli>(t1 - t0).count(),
		std::chrono::duration<double, std::milli>(t2 - t1).count(),
		std::chrono::duration<double, std::milli>(t3 - t2).count(),
		std::chrono::duration<double, std::milli>(t4 - t3).count(),
		std::chrono::duration<double, std::milli>(t5 - t4).count()
	);*/
}

void Application::Shutdown() {
	std::cout << "Application shutting down\n";
	m_Render_System.ShutDown_DebugRender();
	if (m_testWindow) {

		m_testWindow->Shutdown();
		m_DebugUI->ShutDownUI();
	}
	m_FrameBuffer->Destory();
	m_testWindow.reset();
	m_running = false;

}

Application::~Application() {
	//Shutdown();  

}