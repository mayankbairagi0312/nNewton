//========================================> Mayank Bairagi <===========================================//
//========================================|> nNewtonText  <|===========================================//
//=========================================> Version 0.1 <==========================//
#pragma once
#include <iostream>
#include <memory>
#include "Input.h"
#include "Renderer/DebugRenderer.hpp"
#include "Renderer/RenderSystem.hpp"
#include "DebugUI/DebugUI.hpp"
#include <nNewton/nMath.hpp>
#include <nNewton/nCollision.hpp>
#include "PhysicsSystem/PhysicsSystem.hpp"
#include "Renderer/GL_framebuffer.hpp"

class Application {
private:
	Camera m_camera;
	std::unique_ptr<Window> m_testWindow;
	Input m_input;
	std::unique_ptr<PhysicsSystem> m_physicsSystem;
	std::unique_ptr<nRenderSystem> m_RenderSystem;
	std::unique_ptr<DebugUIEditor> m_DebugUI;

	bool m_running;
	Uint64 m_CurrTime;
	Uint64 m_PrevTime;
	float m_DeltaTime;

public:
	// -- Constructors --
	Application();
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;
	Application(Application&&) noexcept = delete;
	Application& operator=(Application&&) noexcept = delete;
	~Application();

	// -- Lifecycle --
	int Run();
	void Shutdown();

private:
	void TRender();
};
