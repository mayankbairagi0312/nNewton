//========================================> Mayank Bairagi <===========================================//
//========================================|> nNewtonText  <|===================================================//
//=========================================> Version 0.1 <=========================================//
#pragma once
#include <iostream>
#include "Input.h"
#include "Renderer/DebugRenderer.hpp"
#include"Renderer/GL_Debug_Renderer.cpp"
#include "DebugUI/DebugUI.cpp"

class Application {
private :

	std::unique_ptr<Window> m_testWindow;
	Input m_input;
	bool m_running;

	DebugRenderer m_TestDebugRenderer;
	//OpneGLDebugRenderer* m_debugDrawer;
	std::unique_ptr<OpneGLDebugRenderer> m_debugDrawer;
	std::unique_ptr<DebugUIEditor> m_DebugUI = std::make_unique<DebugUIEditor>();
	Camera m_camera;

	Uint64 m_CurrTime;
	Uint64 m_PrevTime;
	double m_DeltaTime;

public : 

	Application();
	~Application();
	bool Initialize();
	void Run();
	
	void Shutdown();

private: 
	//void Update();
	void TRender();
	


};