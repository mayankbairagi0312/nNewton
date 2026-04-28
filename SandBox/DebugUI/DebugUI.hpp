#include <imgui.h>
#include<imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>
#include "Core/Window.h"
#include"Renderer/DebugRenderer.hpp"
#include <memory>
#include <nNewton/nCollision.hpp>
#include<nNewton/nAABBTree.hpp>


class DebugUIEditor {

private:
	Window* SDL_Window;
	std::shared_ptr<DebugRenderer>  debugRenderer;
	nNewton::nCollisionWorld* m_CollisionWorld;
	bool m_DrawStatic = true;
	bool m_DrawDynamic = true;
	bool m_DrawFatAABB = false;
	int  m_DebugMaxDepth = 5;

public:
	bool DrawStatic()    const { return m_DrawStatic; }
	bool DrawDynamic()   const { return m_DrawDynamic; }
	bool DrawFatAABB()   const { return m_DrawFatAABB; }
	int  GetMaxDepth()   const { return m_DebugMaxDepth; }

	bool Init_DebugUIEditor(Window* window, std::shared_ptr<DebugRenderer> render, nNewton::nCollisionWorld* collisionW);
	void BeginUIFrame();
	void EndUIFrame();
	void ShutDownUI();
	void Stats_Overlay(bool* IsOverlay);
	void DrawBVHStats();
};

