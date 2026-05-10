#pragma once 

#include <imgui.h>
#include<imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>
#include "Core/Window.h"
#include"Renderer/DebugRenderer.hpp"
#include <memory>
#include "Renderer/RenderSystem.hpp"
#include<nNewton/nAABBTree.hpp>
#include <nNewton/nTransform.hpp>
#include<nNewton/nDynamicsWorld.hpp>
#include "PhysicsSystem/PhysicsSystem.hpp"
#include"common.hpp"


struct EntityMeta {
	nNewton::nEntity_ID id;
	std::string         name;
	bool                visible = true;
};


class DebugUIEditor {

private:
	Window* SDL_Window;
	std::shared_ptr<DebugRenderer>  debugRenderer;
	nNewton::nDynamicsWorld* m_World;
	nRenderSystem* m_RenderSystem = nullptr;

	std::vector<EntityMeta>        m_Entities;
	nNewton::nEntity_ID            m_SelectedID = {};       
	char                           m_RenameBuffer[64] = {};


	char                           m_NewName[64] = "Entity";
	float                          m_NewMass = 1.0f;
	float                          m_NewPos[3] = {};
	bool						   m_NewIsStatic = false;;
	float						   m_NewVelocity[3] = { 0.f, 0.f, 0.f };
	float						   m_NewScale[3] = { 1.f, 1.f, 1.f };

	int                            m_NewShapeType = 0;       
	float                          m_NewHalfExt[3] = { 0.5f, 0.5f, 0.5f };
	float                          m_NewRadius = 0.5f;
	nVector4 m_NewColor = { nColor::Magenta.r,nColor::Magenta.g,nColor::Magenta.b,nColor::Magenta.a };


	float m_EditPos[3] = {};
	float m_EditRot[3] = {};  
	float m_EditScale[3] = { 1,1,1 };

	SimState m_SimState = SimState::Stopped;
	bool     m_StepRequested = false;

	struct TransformSnapshot {
		nNewton::nEntity_ID id;
		float pos[3];
		float rot[3];   
	};
	std::vector<TransformSnapshot> m_PlaySnapshot;

	void DrawEntityList();
	void DrawAddEntityPopup();
	void DrawInspector();
	void DrawTransformSection();
	void DrawPhysicsSection();
	//void DrawPlayBar();

	EntityMeta* FindMeta(nNewton::nEntity_ID id);
	const EntityMeta* FindMeta(nNewton::nEntity_ID id) const;

	void SyncEditCacheFromWorld();          
	void FlushEditCacheToWorld();

	static const char* ShapeTypeName(int t);

public:
	
	bool Init_DebugUIEditor(Window* window, std::shared_ptr<DebugRenderer> render, nNewton::nDynamicsWorld* ,
		nRenderSystem* renderSystem);
	void BeginUIFrame();
	void EndUIFrame();
	void ShutDownUI();
	void Stats_Overlay(bool* IsOverlay);
	void DrawBVHStats();
	void DrawBVHStatsInline();

	bool IsPlaying() const { return m_SimState == SimState::Playing; }
	bool IsPaused()  const { return m_SimState == SimState::Paused; }
	SimState GetSimState() const { return m_SimState; }

	bool TickSimulation(float deltaTime);
	nNewton::nEntity_ID   GetSelectedID()      const { return m_SelectedID; }

	


	void DrawEditorPanel(bool* open = nullptr);
	

};

