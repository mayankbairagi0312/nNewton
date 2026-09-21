#pragma once 

#include <imgui.h>
#include<imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>
#include "Core/Window.h"
#include"Renderer/DebugRenderer.hpp"
#include <memory>
#include <utility>
#include "Renderer/RenderSystem.hpp"
#include<nNewton/nAABBTree.hpp>
#include <nNewton/nTransform.hpp>
#include<nNewton/nDynamicsWorld.hpp>
#include "PhysicsSystem/PhysicsSystem.hpp"
#include"common.hpp"
#include "Renderer/GL_framebuffer.hpp"
#include <format>
#include "Core/Entity.h"
#include "Core/EntityManager.h"
#include "Core/Component.h"

static bool CaseInsensitiveMatch(std::string_view str1, std::string_view str2);
static std::string Strtrim(const std::string& str);
static bool CaseInsensitiveMatchStart(std::string_view str, std::string_view pref);


class DebugUIEditor;

//======== Console ------------------------------------------------------------
//----------------------------------------------------------------------------
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


struct Editor_Entity {
	nNewton::nEntity_ID id;
	int folderId = -1;
};

//--------------------------------------------------------------------------------

class DebugUIEditor {

private:

//---------------------------------------------------------------
	std::unique_ptr<SandboxFramebuffer> m_FrameBuff;
	Window* SDL_Window;
	DebugRenderer*  debugRenderer;
	nNewton::nDynamicsWorld* m_World = nullptr;
	std::unique_ptr<eManager> m_EntityManager;
	nRenderSystem* m_RenderSystem = nullptr;
	



	

	size_t m_EntitiesCount = 0;
	nNewton::nEntity_ID            m_SelectedID = {};       
	char                           m_RenameBuffer[64] = {};

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
	nNewton::nEntity_ID m_RenamingEntityID = nNewton::INVALID_ENTITY;
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


	ImVec2      m_ViewportSize = { 1280.0f, 720.0f };
	bool        m_ViewportFocused = false;
	bool        m_ViewportHovered = false;
	bool		m_ProcessMouseInput = false;


	char                           m_NewName[64] = "Entity";
	    
	float                          m_NewHalfExt[3] = { 1.0f, 1.0f, 1.0f };
	float                          m_NewRadius = 1.0f;
	nCollisionEntity m_Collider;


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



//------------draw func-----------------
	void DrawFolderNode(Editor_FolderNode& node);
	void DrawEntityRow(Editor_Entity& meta, int parentFolderID);
	void DrawFolderNodeTree();
	void DrawAddEntityPopup();
	void DrawInspector();
	void DrawTransformSection();
	void DrawPhysicsSection();
	//void DrawPlayBar();

//--------------Support-------------
	Editor_Entity* FindMetaEntity(nNewton::nEntity_ID id);
	const Editor_Entity* FindMetaEntity(nNewton::nEntity_ID id) const;


	void AddFolder();
	void RenameFolder();
	void RenameEntity();
	void AddSubFolder(int parentID, const std::string& name);
	void RemoveFolder(int FolderId);
	void MoveEntityToFolder(int entityId, int TargetFolderID);
	//void MoveFolderToFolder(int entityId, int folderParentID);
	void RemoveEntityNodeTree(nNewton::nEntity_ID id);

	void SyncEditCacheFromComponent(const TransformComponent* tf);
	void FlushEditCacheToComponent(TransformComponent* tf);
	void FlushComponentToWorld(const TransformComponent* tf);

	static const char* ShapeTypeName(int t);


	template <typename ComponentType, typename DrawFunc>
	void DrawComponentCard(const char* componentName, DrawFunc&& drawContent, float height = 200.0f);
	template <typename ComponentType, typename DrawFunc, typename OnRemoveFunc>
	void DrawComponentCard(const char* name, DrawFunc&& draw,
		float height, OnRemoveFunc&& onRemove);
public:
	
	DebugUIEditor(Window* window, nNewton::nDynamicsWorld* world,
		nRenderSystem* renderSystem);
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

	const eManager* GetEntityManager()  const { return m_EntityManager.get(); }
	nNewton::nEntity_ID CreateEntity(const std::string& name,
		const nNewton::nTransform& transform = nTransform());

	static EditorConsole& GetConsole()
	{
		static EditorConsole console;
		return console;
	}
	SandboxFramebuffer* GetFrameBuff() {
		return m_FrameBuff.get();
	}
	static void DrawConsole(bool* p_open);

	template<typename... Args>
	static void AddLog(std::format_string<Args...> fmt, Args&&... args);

	bool DeleteEntity(nEntity_ID id);
	//void DestroyAllEntities();
	bool RebuildBVHTree(bool isStatic);
	
	bool IsViewportFocused()const { return m_ViewportFocused; }
	bool IsViewportHovered()const { return m_ViewportHovered; }
	bool IsProcessMouse()const { return m_ProcessMouseInput; }
};


template <typename ComponentType, typename DrawFunc>
void DebugUIEditor::DrawComponentCard(const char* componentName, DrawFunc&& drawContent, float height)
{
	DrawComponentCard<ComponentType>(componentName, std::forward<DrawFunc>(drawContent), height, [] {});
}

template<typename ComponentType, typename DrawFunc, typename OnRemoveFunc>
inline void DebugUIEditor::DrawComponentCard(const char* name, DrawFunc&& draw, float height, OnRemoveFunc&& onRemove)
{
	bool alive = true;
	if (m_EntityManager->HasComponent<ComponentType>(m_SelectedID))
	{
		if (ImGui::BeginChild(name, ImVec2(0, height),
			ImGuiChildFlags_Borders, ImGuiWindowFlags_NoScrollbar))
		{
			ImGui::Text("%s", name);
			ImGui::SameLine();

			float avail = ImGui::GetContentRegionAvail().x;
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + avail
				- ImGui::CalcTextSize(" X ").x
				- ImGui::GetStyle().FramePadding.x * 2);

			ImGui::PushStyleColor(ImGuiCol_Button, { 0.8f, 0.2f, 0.2f, 1.f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 1.0f, 0.3f, 0.3f, 1.f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.6f, 0.1f, 0.1f, 1.f });
			ImGui::PushID("rem");
			if (ImGui::SmallButton(" X ")) alive = false;
			ImGui::PopID();
			ImGui::PopStyleColor(3);

			ImGui::Separator();
			draw();
		}
		ImGui::EndChild();

		if (!alive)
		{
			onRemove();                                          
			m_EntityManager->RemoveComponent<ComponentType>(m_SelectedID);
		}
	}
}



