//======================================== Mayank Bairagi ===========================================//
//========================================?? nNewton demo application ??===================================================//
//=========================================== version 0.1 =========================================//
#pragma once
#include <iostream>
#include "Input.h"
#include "DebugRenderer.hpp"
#include"GL_Debug_Renderer.cpp"
#include<imgui.h>
class Application {
private :

	std::unique_ptr<Window> m_testWindow;
	Input m_input;
	bool m_running;

	DebugRenderer m_TestDebugRenderer;
	//OpneGLDebugRenderer* m_debugDrawer;
	std::unique_ptr<OpneGLDebugRenderer> m_debugDrawer;
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