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
#include "Renderer/GL_framebuffer.hpp"
#include <format>


static bool CaseInsensitiveMatch(std::string_view str1, std::string_view str2);
static std::string Strtrim(const std::string& str);
static bool CaseInsensitiveMatchStart(std::string_view str, std::string_view pref);
struct Editor_Entity {
	nNewton::nEntity_ID id;
	std::string         name;
	bool                visible = true;
	int folderId = -1;
};



class DebugUIEditor;
//======== Console 

class EditorConsole
{
private:
	struct LogEntry {
		std::string Text;
		ImVec4 Color;
		bool HasColor;
	};

public:
	EditorConsole();
	~EditorConsole();

	void ClearLog();
	void AddLog(char* buf);
	void    Draw(const char* title, bool* p_open);
	void    ExecCommand(const std::vector<std::string>& command_line);
	int TextEditCallback(ImGuiInputTextCallbackData* data);
	void SetEditorForConsole(DebugUIEditor* UI) { m_Owner = UI; }
private:

	char							InputBuf[256];
	std::vector<LogEntry>			Items;
	std::vector<const char*>		Commands;
	std::vector<int>				FilterIndices;
	std::vector<std::string>		History;
	int								HistoryPos;
	ImGuiTextFilter					Filter;
	bool							FilterDirty;
	bool							AutoScroll;
	bool							ScrollToBottom;
	using CommandHandler = std::function<void(const std::vector<std::string>&)>;
	std::unordered_map<std::string, CommandHandler> m_CommandMap;
	DebugUIEditor* m_Owner = nullptr;

	void UpdateFilter()
	{
		FilterIndices.clear();

		for (int i = 0; i < Items.size(); ++i)
		{
			if (Filter.PassFilter(Items[i].Text.c_str())) FilterIndices.push_back(i);
		}
		FilterDirty = false;
	}
	void RegisterCommands();
};


class DebugUIEditor {

private:
	Window* SDL_Window;
	std::shared_ptr<DebugRenderer>  debugRenderer;
	nNewton::nDynamicsWorld* m_World;
	nRenderSystem* m_RenderSystem = nullptr;

	SandboxFramebuffer* m_FrameBuff;

	size_t m_EntitiesCount = 0;
	nNewton::nEntity_ID            m_SelectedID = {};       
	char                           m_RenameBuffer[64] = {};
	Editor_Entity* m_RenamingEntity = nullptr;

	//---------entity list floder
	struct Editor_FolderNode
	{
		std::string folderName;
		int floderID ;
		std::vector<Editor_FolderNode> ChildFolders;
		std::vector<Editor_Entity> ChildEntities;
		
		
	};

	std::vector<Editor_FolderNode> m_RootFolders;
	std::vector<Editor_Entity> m_RootEntities;
	int m_NextFolderID = 0;               
	Editor_FolderNode* m_RenamingFolder = nullptr;
	char m_RenamingFolderBuf[64];
	int m_FolderToDelete;
	bool m_NeedRebuildTree;
	int m_ActiveFolderID = -1;
	int m_FolderPopupParentID = -1;
	char m_NewFolderName[64];
	bool m_OpenRenameFolderPopup = false;
	bool m_OpenRenameEntityPopup = false;
	bool isAddFolderPopUp = false;
	void DrawFolderNode(Editor_FolderNode& node);
	void DrawEntityRow(Editor_Entity& meta, int parentFolderID);
	void AddFolder();
	void RenameFolder();
	void RenameEntity();
	void AddSubFolder(int parentID, const std::string& name);
	void RemoveFolder(int FolderId);
	void MoveEntityToFolder(int entityId, int TargetFolderID);
	//void MoveFolderToFolder(int entityId, int folderParentID);
	void RemoveEntityNodeTree(nNewton::nEntity_ID id);



	ImVec2      m_ViewportSize = { 1280.0f, 720.0f };
	bool        m_ViewportFocused = false;
	bool        m_ViewportHovered = false;
	bool		m_ProcessMouseInput = false;


	char                           m_NewName[64] = "Entity";
	float                          m_NewMass = 1.0f;
	float                          m_NewPos[3] = {};
	float						   m_NewRotation[3] = {};
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

	void DrawFolderNodeTree();
	void DrawAddEntityPopup();
	void DrawInspector();
	void DrawTransformSection();
	void DrawPhysicsSection();
	//void DrawPlayBar();

	Editor_Entity* FindMetaEntity(nNewton::nEntity_ID id);
	const Editor_Entity* FindMetaEntity(nNewton::nEntity_ID id) const;

	void SyncEditCacheFromWorld();          
	void FlushEditCacheToWorld();

	static const char* ShapeTypeName(int t);

public:
	
	bool Init_DebugUIEditor(Window* window, std::shared_ptr<DebugRenderer> render, nNewton::nDynamicsWorld* ,
		nRenderSystem* renderSystem,SandboxFramebuffer* FrameBuff);
	void BeginUIFrame();
	void EndUIFrame();
	void ShutDownUI();

	void RenderUI( bool* IsPanels);

	void BeginDockspace();
	void EndDockspace();
	void ViewportBegin(Camera* camera);
	void ViewportEnd(bool* IsOverlay);

	void DrawBVHStatsInline();
	void ApplyCustomStyle();

	bool IsPlaying() const { return m_SimState == SimState::Playing; }
	bool IsPaused()  const { return m_SimState == SimState::Paused; }
	SimState GetSimState() const { return m_SimState; }

	bool TickSimulation(float deltaTime);
	nNewton::nEntity_ID   GetSelectedID()      const { return m_SelectedID; }

	
	void DrawWorldOutlinerPanel(bool* open);
	void DrawPropertiesPanel(bool* open);
	void DrawDiagnosticsPanel(bool* open);

	static EditorConsole& GetConsole()
	{
		static EditorConsole console;
		return console;
	}
	static void DrawConsole(bool* p_open);

	template<typename... Args>
	static void AddLog(std::format_string<Args...> fmt, Args&&... args);

	void defaultScene();

	nEntity_ID CreateEntity(const std::string& name, float mass = 1.0f, bool isStatic = true, nNewton::nCollisionShapeType shape = nNewton::nCollisionShapeType::Box,
		const nNewton::nTransform& Transform = nNewton::nTransform(),
		const nNewton::nVector3& init_velocity = nNewton::nVector3(),
		const nNewton::nVector4& color = { 0.2f,0.7f,0.8f,1.0f });
	nEntity_ID CreateEntityRand(bool isStatic);
	bool DeleteEntity(nEntity_ID id);
	void DestroyAllEntities();
	bool RebuildBVHTree(bool isStatic);
	
	bool IsViewportFocused()const { return m_ViewportFocused; }
	bool IsViewportHovered()const { return m_ViewportHovered; }
	bool IsProcessMouse()const { return m_ProcessMouseInput; }
};



