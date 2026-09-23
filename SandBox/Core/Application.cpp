#include "Application.h"
#include <chrono>

Application::Application()
	: m_testWindow(std::make_unique<Window>())
	, m_input(m_testWindow.get())
	, m_physicsSystem(std::make_unique<PhysicsSystem>())
	, m_RenderSystem(std::make_unique<nRenderSystem>(&m_camera, m_physicsSystem->GetPhysicsWorld()))
	, m_DebugUI(std::make_unique<DebugUIEditor>(m_testWindow.get(), m_physicsSystem->GetPhysicsWorld(), m_RenderSystem.get()))
	, m_running(false)
	, m_CurrTime(0)
	, m_PrevTime(SDL_GetPerformanceCounter())
	, m_DeltaTime(0.0f)
{
	m_input.SetCamera(m_camera);
	m_running = true;
	std::cout << "=== > Application initialized successfully < ===.\n";
	DebugUIEditor::AddLog("===> Application initialized Successfully <===");
	DebugUIEditor::AddLog("===>        Welcome to nNewton            <===");
}

int Application::Run()
{
	if (!m_testWindow || !m_testWindow->IsValid()) {
		std::cerr << "Cannot run: Window is not valid!" << std::endl;
		return EXIT_FAILURE;
	}

	m_PrevTime = SDL_GetPerformanceCounter();
	std::cout << "Application starting main loop...\n";

	while (m_running)
	{
		// Frame timing.
		m_CurrTime = SDL_GetPerformanceCounter();
		m_DeltaTime = static_cast<float>(
			(m_CurrTime - m_PrevTime) / static_cast<double>(SDL_GetPerformanceFrequency())
			);
		m_PrevTime = m_CurrTime;

		m_input.BeginFrame();

		// Event pump.
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			ImGui_ImplSDL3_ProcessEvent(&event);
			if (event.type == SDL_EVENT_QUIT) {
				m_running = false;
			}
			m_input.ProcessEvent(&event);
		}

		// Physics step.
		SandboxFramebuffer::ClearFrameBuff();
		m_physicsSystem->UpdatePhysicsSystem(m_DeltaTime);

		// UI frame.
		m_DebugUI->BeginUIFrame();
		m_DebugUI->ViewportBegin(&m_camera);

		TRender();

		static bool demo = true;
		m_DebugUI->ViewportEnd(&demo);

		if (m_DebugUI->IsProcessMouse()) m_input.ProcessMouseInput();
		if (m_DebugUI->IsViewportFocused() && m_DebugUI->IsViewportHovered()) {
			m_input.ProcessInputKey(m_DeltaTime);
		}

		ImGui::ShowDemoWindow(&demo);
		m_DebugUI->RenderUI(&demo);

		m_DebugUI->EndUIFrame();

		SDL_GL_SwapWindow(m_testWindow->GetNativeHandle());
		m_input.EndFrame();
	}

	std::cout << "[INFO] Application main loop exited cleanly.\n";
	return EXIT_SUCCESS;
}

void Application::TRender()
{
	m_DebugUI->GetFrameBuff()->Bind();

	m_RenderSystem->Start_Debug_Draw();
	m_RenderSystem->GetRenderer()->SetFlagEnabled(flags::Shapes);
	m_RenderSystem->Debug_DrawAxis(m_camera.GetPosition());
	m_RenderSystem->Debug_Render();
	m_RenderSystem->End_Debug_Draw();

	m_DebugUI->GetFrameBuff()->Unbind();
}

void Application::Shutdown()
{
	std::cout << "Application shutting down\n";
	m_RenderSystem->ShutDown_DebugRender();
	if (m_testWindow) {
		m_DebugUI->ShutDownUI();
		m_testWindow->Shutdown();
	}

	m_testWindow.reset();
	m_running = false;
}

Application::~Application()
{
	Shutdown();
}
