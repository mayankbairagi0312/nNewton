#include "DebugUI.hpp"
#include <algorithm>
#include <random>
#include <cstring>
#ifdef _MSC_VER
#define Stricmp _stricmp
#else
#define Stricmp strcasecmp
#endif

bool DebugUIEditor::Init_DebugUIEditor(Window* window, std::shared_ptr<DebugRenderer> render, nNewton::nDynamicsWorld* world,
	nRenderSystem* renderSystem , SandboxFramebuffer* FrameBuff)
{
	SDL_Window = window;
	debugRenderer = render;
	m_World = world;
	m_FrameBuff = FrameBuff;
	std::cout << "debug render this=" << debugRenderer.get() << std::endl;
	m_RenderSystem = renderSystem;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsClassic();
	ApplyCustomStyle();
	ImGuiIO& io = ImGui::GetIO();

	io.Fonts->AddFontFromFileTTF("assets/Fonts/JetBrainsMonoNL-Regular.ttf", 13.0f);
	
	io.FontGlobalScale = 2.0f;

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	
	ImGui_ImplSDL3_InitForOpenGL(SDL_Window->GetNativeHandle(), SDL_Window->GetSDLglContext());
	ImGui_ImplOpenGL3_Init("#version 460");
	
	m_FrameBuff->Initialize(m_ViewportSize.x,m_ViewportSize.y);

	GetConsole().SetEditorForConsole(this);
	//defaultScene();
	
	return true;

}

void DebugUIEditor::ApplyCustomStyle()
{
	ImGuiStyle& s = ImGui::GetStyle();

	//Rounding 
	s.WindowRounding = 2.0f;
	s.FrameRounding = 2.0f;
	s.PopupRounding = 2.0f;
	s.ScrollbarRounding = 2.0f;
	s.GrabRounding = 2.0f;
	s.TabRounding = 2.0f;
	s.ChildRounding = 2.0f;

	//Spacing 
	s.WindowPadding = ImVec2(8, 8);
	s.FramePadding = ImVec2(5, 4);
	s.ItemSpacing = ImVec2(8, 5);
	s.ItemInnerSpacing = ImVec2(4, 4);
	s.IndentSpacing = 14.0f;
	s.ScrollbarSize = 14.0f;
	s.GrabMinSize = 10.0f;

	//Borders
	s.WindowBorderSize = 1.0f;
	s.ChildBorderSize = 1.0f;
	s.PopupBorderSize = 1.0f;
	s.FrameBorderSize = 1.0f;

	ImVec4* c = s.Colors;

	// Darks
	ImVec4 baseBg = ImVec4(0.1f, 0.1f, 0.1f, 1.00f);
	ImVec4 panelBg = ImVec4(0.12f, 0.13f, 0.13f, 1.00f);
	ImVec4 borderCol = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);

	// Accent
	ImVec4 accent = ImVec4(0.24f, 0.36f, 0.48f, 1.00f);
	ImVec4 accentHover = ImVec4(0.31f, 0.44f, 0.58f, 1.00f);
	ImVec4 accentActive = ImVec4(0.19f, 0.29f, 0.40f, 1.00f);

	// Text - Soft off-white
	c[ImGuiCol_Text] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
	c[ImGuiCol_TextDisabled] = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);

	// Windows & Panels
	c[ImGuiCol_WindowBg] = baseBg;
	c[ImGuiCol_ChildBg] = panelBg;
	c[ImGuiCol_PopupBg] = baseBg;
	c[ImGuiCol_Border] = borderCol;
	c[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

	// Frames
	c[ImGuiCol_FrameBg] = panelBg;
	c[ImGuiCol_FrameBgHovered] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	c[ImGuiCol_FrameBgActive] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);

	// Headers
	c[ImGuiCol_Header] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
	c[ImGuiCol_HeaderHovered] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	c[ImGuiCol_HeaderActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);

	// Buttons
	c[ImGuiCol_Button] = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
	c[ImGuiCol_ButtonHovered] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
	c[ImGuiCol_ButtonActive] = accentActive;

	// Tabs
	c[ImGuiCol_Tab] = baseBg;
	c[ImGuiCol_TabHovered] = accentHover;
	c[ImGuiCol_TabActive] = panelBg; 
	c[ImGuiCol_TabUnfocused] = baseBg;
	c[ImGuiCol_TabUnfocusedActive] = panelBg;

	// Title
	c[ImGuiCol_TitleBg] = baseBg;
	c[ImGuiCol_TitleBgActive] = baseBg;
	c[ImGuiCol_TitleBgCollapsed] = baseBg;
	c[ImGuiCol_MenuBarBg] = baseBg;

	// Scrollbars
	c[ImGuiCol_ScrollbarBg] = baseBg;
	c[ImGuiCol_ScrollbarGrab] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
	c[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
	c[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);

	c[ImGuiCol_CheckMark] = accentHover;
	c[ImGuiCol_CheckboxSelectedBg] = panelBg;
	c[ImGuiCol_SliderGrab] = accent;
	c[ImGuiCol_SliderGrabActive] = accentActive;

	// Resize Grips
	c[ImGuiCol_ResizeGrip] = ImVec4(0.18f, 0.18f, 0.18f, 0.50f);
	c[ImGuiCol_ResizeGripHovered] = accentHover;
	c[ImGuiCol_ResizeGripActive] = accentActive;

	// Separators
	c[ImGuiCol_Separator] = borderCol;
	c[ImGuiCol_SeparatorHovered] = accentHover;
	c[ImGuiCol_SeparatorActive] = accentActive;
}


void DebugUIEditor::BeginUIFrame()
{

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL3_NewFrame();

	ImGui::NewFrame();
	BeginDockspace();

}
void DebugUIEditor::EndUIFrame()
{
	EndDockspace();
	ImGui::Render();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void DebugUIEditor::ShutDownUI()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();
}

void DebugUIEditor::RenderUI(bool* IsPanels )
{	
		DrawEntityListPanel(IsPanels);
		DrawPropertiesPanel(IsPanels);
		DrawConsole(IsPanels);
		DrawDiagnosticsPanel(IsPanels);
}


void DebugUIEditor::BeginDockspace()
{
	
	ImGuiWindowFlags flags =
		ImGuiWindowFlags_MenuBar |
		ImGuiWindowFlags_NoDocking |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

	const ImGuiViewport* vp = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(vp->WorkPos);
	ImGui::SetNextWindowSize(vp->WorkSize);
	ImGui::SetNextWindowViewport(vp->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin("##MainDockspace", nullptr, flags);
	ImGui::PopStyleVar(3);

	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			ImGui::MenuItem("New Scene", "Ctrl+N");
			ImGui::MenuItem("Open Scene", "Ctrl+O");
			ImGui::MenuItem("Save", "Ctrl+S");
			ImGui::Separator();
			ImGui::MenuItem("Exit");
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Debug")) {
			ImGui::MenuItem("Stats Overlay");
			ImGui::MenuItem("BVH Debug");
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	ImGuiID dockID = ImGui::GetID("MainDockSpace");
	ImGui::DockSpace(dockID, ImVec2(0, 0), ImGuiDockNodeFlags_None);
}

void DebugUIEditor::EndDockspace()
{
	ImGui::End(); 
}

void DebugUIEditor::ViewportBegin(Camera* camera)
{
	if (!camera) return;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin("Scene Viewport",nullptr, ImGuiWindowFlags_NoScrollbar);
	ImGui::PopStyleVar();
	
	m_ViewportSize = ImGui::GetContentRegionAvail();

	// Resize framebuf
	if (m_ViewportSize.x > 1 && m_ViewportSize.y > 1) {
		if ((int)m_ViewportSize.x != m_FrameBuff->Width() ||
			(int)m_ViewportSize.y != m_FrameBuff->Height()) {
			m_FrameBuff->Resize((int)m_ViewportSize.x, (int)m_ViewportSize.y);
			m_FrameBuff->Bind();
			glViewport(0, 0, (int)m_ViewportSize.x, (int)m_ViewportSize.y);
			glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			m_FrameBuff->Unbind();

			auto ar = static_cast<float>(m_ViewportSize.x) / static_cast<float>(m_ViewportSize.y);
			camera->setAspectRatio(ar);
		}
	}	
}
void DebugUIEditor::ViewportEnd(bool* IsOverlay) {
	

	m_ViewportFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
	m_ViewportHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);

	ImGui::Image(
		(ImTextureID)(uintptr_t)m_FrameBuff->getFrameTexture(),
		m_ViewportSize,
		ImVec2(0, 1),   // top-left UV
		ImVec2(1, 0)    // bottom-right UV
	);

	bool imageHovered = ImGui::IsItemHovered();
	auto MousePos = ImGui::GetMousePos();

	m_ProcessMouseInput =
		imageHovered &&
		m_ViewportFocused ;

//========================================
	ImVec2 vpMin = ImGui::GetItemRectMin();
	ImVec2 vpMax = ImGui::GetItemRectMax();

	ImVec2 viewportPos = vpMin;
	ImVec2 viewportSize = ImVec2(
		vpMax.x - vpMin.x,
		vpMax.y - vpMin.y
	);

	ImDrawList* drawList = ImGui::GetWindowDrawList();

	ImVec2 overlayPos = ImVec2(
		viewportPos.x + 10.0f,
		viewportPos.y + 10.0f
	);
	ImVec2 overlaySize = ImVec2(260, 180);

	ImGuiIO& io = ImGui::GetIO();

	char buffer[128];

	sprintf(buffer, "FPS: %.1f", io.Framerate);

	drawList->AddText(
		ImVec2(overlayPos.x + 10, overlayPos.y + 10),
		IM_COL32_WHITE,
		buffer
	);

	sprintf(buffer, "Frame Time: %.2f ms", 1000.0f / io.Framerate);

	drawList->AddText(
		ImVec2(overlayPos.x + 10, overlayPos.y + 32),
		IM_COL32_WHITE,
		buffer
	);
	
	if(ImGui::IsMousePosValid())
		sprintf(buffer, "Mouse Position :(%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
	else
		sprintf(buffer, "Mouse Position : <invalid>");

	drawList->AddText(
		ImVec2(overlayPos.x + 10, overlayPos.y + 54),
		IM_COL32_WHITE,
		buffer
	);

	sprintf(buffer, "Entity Count: %d",(int)m_Entities.size());

	drawList->AddText(
		ImVec2(overlayPos.x + 10, overlayPos.y + 76),
		IM_COL32_WHITE,
		buffer
	);

	sprintf(buffer, "Vertex : %d", debugRenderer->GetLineCount()*2);

	drawList->AddText(
		ImVec2(overlayPos.x + 10, overlayPos.y + 98),
		IM_COL32_WHITE,
		buffer
	);
//============================================

	ImGui::SetCursorPos(ImVec2(vpMax.x - 80, 28));
	ImGui::Button("T"); ImGui::SameLine();
	ImGui::Button("R"); ImGui::SameLine();
	ImGui::Button("S");

	ImGui::End();
}


void DebugUIEditor::DrawDiagnosticsPanel(bool* open)
{
	if (!ImGui::Begin("Diagnostics", open)) {
		ImGui::End();
		return;
	}

	ImGui::Spacing();

	{
		static float values[90] = {};
		static int values_offset = 0;
		static double refresh_time = 0.0;

		if (refresh_time == 0.0)
			refresh_time = ImGui::GetTime();

		while (refresh_time < ImGui::GetTime())
		{
			values[values_offset] = ImGui::GetIO().Framerate;
			values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);
			refresh_time += 1.0f / 60.0f;
		}

		float average = 0.0f;
		for (int n = 0; n < IM_ARRAYSIZE(values); n++)
			average += values[n];

		average /= (float)IM_ARRAYSIZE(values);

		char overlay[32];
		sprintf(overlay, "Avg FPS: %.1f FPS", average);

		ImGui::PlotLines("Framerate", values, IM_ARRAYSIZE(values), values_offset,
			overlay, 0.0f, 2000.0f, ImVec2(0, 80.0f)
		);
	}


	{
		static float values[90] = {};
		static int values_offset = 0;
		static double refresh_time = 0.0;

		if (refresh_time == 0.0)
			refresh_time = ImGui::GetTime();

		while (refresh_time < ImGui::GetTime())
		{
			values[values_offset] = 1000.0f / ImGui::GetIO().Framerate;
			values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);
			refresh_time += 1.0f / 60.0f;
		}

		float average = 0.0f;
		for (int n = 0; n < IM_ARRAYSIZE(values); n++)
			average += values[n];

		average /= (float)IM_ARRAYSIZE(values);

		char overlay[32];
		sprintf(overlay, "Avg Frametime: %.1f FPS", average);

		ImGui::PlotLines("Frametime", values, IM_ARRAYSIZE(values), values_offset,
			overlay, 0.0f, 10.0f, ImVec2(0, 80.0f)
		);
	}

	ImGui::Spacing();

	DrawBVHStatsInline();

	ImGui::End();
}




bool DebugUIEditor::TickSimulation(float deltaTime)
{
	bool stepped = false;

	if (m_SimState == SimState::Playing) {
		//m_World->Step(deltaTime);
		stepped = true;
	}
	else if (m_StepRequested) {
		//m_World->Step(deltaTime);   
		stepped = true;
	}
	m_StepRequested = false;
	return stepped;
}


void DebugUIEditor::DrawEntityListPanel(bool* open) 
{
	
	if (!ImGui::Begin("Entity List", open)) {
		ImGui::End();
		return;
	}
	DrawEntityList();

	ImGui::End();
}

void DebugUIEditor::DrawEntityList()
{
	
	ImGui::TextDisabled("ENTITIES %d ", (int)m_Entities.size());

	float windowWidth = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
	float buttonWidth = ImGui::CalcTextSize("  +  ").x + ImGui::GetStyle().FramePadding.x * 2;
	
	ImGui::SameLine(ImGui::GetWindowContentRegionMax().x  - buttonWidth * 2 - 8.f);

	if (ImGui::SmallButton("Add"))
		ImGui::OpenPopup("AddEntityPopup");
	if (ImGui::IsItemHovered()) ImGui::SetTooltip("Add entity");

	ImGui::SameLine(0, 4);

	bool hasSelection = m_World && m_World->IsValid(m_SelectedID);
	if (!hasSelection) ImGui::BeginDisabled();
	if (ImGui::SmallButton("remove")) {
		m_World->DestroyEntity(m_SelectedID);
		m_Entities.erase(std::remove_if(m_Entities.begin(), m_Entities.end(),
			[this](const EntityMeta& m) { return m.id == m_SelectedID; }),
			m_Entities.end());
		m_SelectedID = {};
	}
	if (!hasSelection) ImGui::EndDisabled();
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
		ImGui::SetTooltip("Destroy selected entity");

	//ImGui::Separator();

	DrawAddEntityPopup();

	float listH = ImGui::GetContentRegionAvail().y ; 
	ImGui::BeginChild("##list_scroll", ImVec2(0, listH));
	int i = 1;
	for (auto& meta : m_Entities)
	{
		// Stale guard 
		if (!m_World->IsValid(meta.id)) continue;

		bool selected = (meta.id == m_SelectedID);

		ImGui::PushID(nNewton::INDEX_FROM_ID(meta.id));  

		// Coloured dot based on mass
		const nNewton::nRigidBody* body = m_World->GetBody(meta.id);
		bool isStatic = body && (body->MASS_ == 0.f);
		ImVec4 dot = isStatic
			? ImVec4(0.45f, 0.75f, 0.45f, 1.f)   
			: ImVec4(0.35f, 0.60f, 0.95f, 1.f);   
		
		ImGui::PushStyleColor(ImGuiCol_Text, dot);
		ImGui::Text(" %d. ", i);
		ImGui::PopStyleColor();
		ImGui::SameLine();

		if (ImGui::Selectable(meta.name.c_str(), selected,
			ImGuiSelectableFlags_SpanAllColumns))
		{
			m_SelectedID = meta.id;

			
			SyncEditCacheFromWorld();
		}

		// Double-click -> rename inline
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
			std::strncpy(m_RenameBuffer, meta.name.c_str(),
				sizeof(m_RenameBuffer));
			ImGui::OpenPopup("##rename");
		}

		if (ImGui::BeginPopup("##rename")) {
			ImGui::SetKeyboardFocusHere();
			if (ImGui::InputText("Name", m_RenameBuffer,
				sizeof(m_RenameBuffer),
				ImGuiInputTextFlags_EnterReturnsTrue)) {
				meta.name = m_RenameBuffer;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		ImGui::PopID();
		i++;
	}

	ImGui::EndChild();

	//DrawPlayBar();
}

void DebugUIEditor::DrawAddEntityPopup()
{
	if (!ImGui::BeginPopup("AddEntityPopup")) return;

	ImGui::TextDisabled("New Entity");
	//ImGui::Separator();

	ImGui::InputText("Name", m_NewName, sizeof(m_NewName));
	ImGui::DragFloat("Mass", &m_NewMass, 0.1f, 0.f, 1000.f, "%.2f kg");
	ImGui::DragFloat3("Position", m_NewPos, 0.1f);
	ImGui::Checkbox("Static", &m_NewIsStatic);

	const char* shapes[] = { "Box", "Sphere", "Capsule" };
	ImGui::Combo("Shape", &m_NewShapeType, shapes, IM_ARRAYSIZE(shapes));

	if (m_NewShapeType == 0)
		ImGui::DragFloat3("Half Extents", m_NewHalfExt, 0.01f, 0.01f, 100.f);
	else
		ImGui::DragFloat("Radius", &m_NewRadius, 0.01f, 0.01f, 100.f);
	
	ImGui::Spacing();

	if (ImGui::Button("Create", ImVec2(80, 0)))
	{
		nNewton::nRigidBodyInfo info{};
		info.MASS_ = m_NewMass;
		info.IS_STATIC_ = m_NewIsStatic;
		info.INIT_VELOCITY_ = { m_NewVelocity[0], m_NewVelocity[1], m_NewVelocity[2] };
		info.INIT_TRANSFORM_.SetPosition({ m_NewPos[0],   m_NewPos[1],   m_NewPos[2] });
		info.INIT_TRANSFORM_.SetRotation(nQuaternion{ 0.f, 0.f, 0.f, 1.f });

		

		switch (m_NewShapeType) {
		case 0:
			info.INIT_TRANSFORM_.SetScale({ m_NewHalfExt[0], m_NewHalfExt[1], m_NewHalfExt[2] });
			info.SetBoxShape({ 1.f, 1.f, 1.f });
			break;
		case 1:
			info.INIT_TRANSFORM_.SetScale({ m_NewRadius, m_NewRadius, m_NewRadius });
			info.SetSphereShape(1.f);
			break;
		}

		nNewton::nEntity_ID newID = m_World->Create_Entity(info,true);


		//m_World->GetCollisionWorld()->CreateCollisionEntity(newID, info.IS_STATIC_, info.INIT_TRANSFORM_, info.INIT_VELOCITY_, info.getCollisionShape());

		if (m_RenderSystem)
			m_RenderSystem->RegisterEntity(newID, m_NewColor);


		EntityMeta meta;
		meta.id = newID;
		meta.name = m_NewName[0] ? m_NewName : "Entity";
		m_Entities.push_back(meta);
		 
		m_SelectedID = newID;
		SyncEditCacheFromWorld();

		
		std::snprintf(m_NewName, sizeof(m_NewName), "Entity");
		m_NewMass = 1.f;
		m_NewPos[0] = m_NewPos[1] = m_NewPos[2] = 0.f;
		m_NewHalfExt[0] = m_NewHalfExt[1] = m_NewHalfExt[2] = 0.5f;
		m_NewScale[0] = m_NewScale[1] = m_NewScale[2] = 1.f;
		m_NewRadius = .5f;
		m_NewIsStatic = false;

		ImGui::CloseCurrentPopup();
	}

	ImGui::SameLine();
	if (ImGui::Button("Cancel", ImVec2(80, 0)))
		ImGui::CloseCurrentPopup();

	

	ImGui::EndPopup();

}


void DebugUIEditor::DrawPropertiesPanel(bool* open)
{
	if (!ImGui::Begin("Properties", open)) {
		ImGui::End();
		return;
	}
	DrawInspector();

	ImGui::End();
}

void DebugUIEditor::DrawInspector()
{
	if (!m_World || !m_World->IsValid(m_SelectedID)) {
		ImVec2 sz = ImGui::GetContentRegionAvail();
		ImGui::SetCursorPos({ sz.x * 0.5f - 55.f, sz.y * 0.5f - 6.f });
		ImGui::TextDisabled("Select an entity");
		return;
	}

	EntityMeta* meta = FindMeta(m_SelectedID);
	const char* displayName = meta ? meta->name.c_str() : "?";

	ImGui::TextDisabled("INSPECTOR");
	ImGui::SameLine();
	ImGui::Text("  %s", displayName);
	ImGui::SameLine();
	ImGui::TextDisabled("[id %u  gen %u]",
		nNewton::INDEX_FROM_ID(m_SelectedID),
		nNewton::GEN_FROM_ID(m_SelectedID));
	ImGui::Separator();

	ImGui::Spacing();
	DrawTransformSection();
	ImGui::Spacing();
	DrawPhysicsSection();
	
	
}

void DebugUIEditor::DrawPhysicsSection()
{
	if (!ImGui::CollapsingHeader("Physics"))
		return;

	const nNewton::nRigidBody* body = m_World->GetBody(m_SelectedID);
	if (!body) { ImGui::TextDisabled("No body"); return; }

	float mass = body->MASS_;

	ImGui::Text("Mass       : %.3f kg", mass);
	ImGui::Text("Type       : %s", mass == 0.f ? "Static" : "Dynamic");

	if (IsPlaying()) {
		auto vel = body->VELOCITY_;   
		ImGui::Text("Lin. vel   : %.2f  %.2f  %.2f", vel.x, vel.y, vel.z);
		/*auto ang = body->;
		ImGui::Text("Ang. vel   : %.2f  %.2f  %.2f", ang.x, ang.y, ang.z);*/
	}

	ImGui::Spacing();

	ImGui::BeginDisabled(IsPlaying());

	static float editMass = 1.f;
	static float editFriction = 0.5f;
	static float editRestitution = 0.f;

	if (ImGui::IsWindowAppearing()) {
		editMass = mass;
		// editFriction = body->GetFriction(); // adapt to your API
	}

	if (ImGui::DragFloat("Mass##edit", &editMass, 0.1f, 0.f, 1e6f))
		/* body->SetMass(editMass); */;   

	ImGui::EndDisabled();
}



void DebugUIEditor::DrawTransformSection()
{
	/*if (m_SimState != SimState::Playing)
		SyncEditCacheFromWorld();*/
	
	ImGui::AlignTextToFramePadding();
	bool open = ImGui::CollapsingHeader("Transform",
		ImGuiTreeNodeFlags_DefaultOpen);

	if (!open) return;

	bool changed = false;

	auto Vec3Row = [&](const char* label, float* v, float speed, const char* fmt) -> bool
		{
			bool dirty = false;
			ImGui::TextDisabled("%s", label);
			ImGui::SameLine(120.f); 

			float itemW = (ImGui::GetContentRegionAvail().x - 8.f) / 3.f;

			struct { const char* prefix; ImVec4 col; } axes[3] = {
				{ "X", { 0.75f, 0.20f, 0.20f, 0.8f } },
				{ "Y", { 0.20f, 0.70f, 0.25f, 0.8f } },
				{ "Z", { 0.20f, 0.40f, 0.80f, 0.8f } },
			};

			for (int i = 0; i < 3; ++i)
			{
				/*ImGui::PushStyleColor(ImGuiCol_FrameBg,
					ImVec4(axes[i].col.x * 0.3f,
						axes[i].col.y * 0.3f,
						axes[i].col.z * 0.3f, 0.6f));*/

				ImGui::SetNextItemWidth(itemW);
				char id[16]; std::snprintf(id, sizeof(id), "##%s%d", label, i);
				if (ImGui::DragFloat(id, &v[i], speed, 0.f, 0.f, fmt))
					dirty = true;

				//ImGui::PopStyleColor();

				if (i < 2) ImGui::SameLine(0, 4);
			}

			return dirty;
		};

	ImGui::Spacing();
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2, 4)); 
	changed |= Vec3Row("Position", m_EditPos, 0.05f, "%.2f");
	changed |= Vec3Row("Rotation", m_EditRot, 0.5f, "%.1f");
	changed |= Vec3Row("Scale", m_EditScale, 0.01f, "%.2f");
	ImGui::PopStyleVar();
	ImGui::Spacing();

	if (ImGui::SmallButton(" Reset ")) {
		std::memset(m_EditPos, 0, sizeof(m_EditPos));
		std::memset(m_EditRot, 0, sizeof(m_EditRot));
		m_EditScale[0] = m_EditScale[1] = m_EditScale[2] = 1.f;
		FlushEditCacheToWorld();
	}


	if (changed)
		FlushEditCacheToWorld();
}


void DebugUIEditor::SyncEditCacheFromWorld()
{
	if (!m_World || !m_World->IsValid(m_SelectedID)) return;

	const nNewton::nTransform* tf = m_World->GetTransform(m_SelectedID);
	if (!tf) return;

	m_EditPos[0] = tf->GetPosition().x;
	m_EditPos[1] = tf->GetPosition().y;
	m_EditPos[2] = tf->GetPosition().z;

	//quat to euler yet to imple
	//   m_EditRot[0] = euler.x;
	//   m_EditRot[1] = euler.y;
	//   m_EditRot[2] = euler.z;

	m_EditScale[0] = tf->GetScale().x;
	m_EditScale[1] = tf->GetScale().y;
	m_EditScale[2] = tf->GetScale().z;
}

void DebugUIEditor::FlushEditCacheToWorld()
{
	if (!m_World || !m_World->IsValid(m_SelectedID)) return;

	nNewton::nRigidBody* body = m_World->GetBody(m_SelectedID);
	if (!body) return;

	
	body->TRANSFORM_.SetPosition({ m_EditPos[0], m_EditPos[1], m_EditPos[2] });

	body->TRANSFORM_.SetScale({ m_EditScale[0], m_EditScale[1], m_EditScale[2] });

	if (body->ColEnt)
		body->ColEnt->EntityTransform = body->TRANSFORM_;
}

EntityMeta* DebugUIEditor::FindMeta(nNewton::nEntity_ID id)
{
	for (auto& m : m_Entities)
		if (m.id == id) return &m;
	return nullptr;
}

const EntityMeta* DebugUIEditor::FindMeta(nNewton::nEntity_ID id) const
{
	for (const auto& m : m_Entities)
		if (m.id == id) return &m;
	return nullptr;
}

const char* DebugUIEditor::ShapeTypeName(int t) {
	switch (t) {
	case 0: return "Box";
	case 1: return "Sphere";
	case 2: return "Capsule";
	default: return "Unknown";
	}
}

void DebugUIEditor::DrawBVHStatsInline()
{
	ImGui::SeparatorText("BVH Tree Stats");	ImGui::Spacing();
	nNewton::nBVHStats dynStats =
		m_World->GetCollisionWorld()->GetDynamicTree()->CollectStats();
	nNewton::nBVHStats staStats =
		m_World->GetCollisionWorld()->GetStaticTree()->CollectStats();

	if (ImGui::TreeNode("Dynamic Tree")) {
		ImGui::Text("Nodes  : %d  Leaves: %d", dynStats.totalNodes, dynStats.leafNodes);
		ImGui::Text("Depth  : %d  Dirty : %d", dynStats.maxDepth, dynStats.dirtyNodes);
		ImGui::Text("Queue  : %d", dynStats.queueSize);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Static Tree")) {
		ImGui::Text("Nodes  : %d  Leaves: %d", staStats.totalNodes, staStats.leafNodes);
		ImGui::Text("Depth  : %d", staStats.maxDepth);
		ImGui::TreePop();
	}

	ImGui::Spacing();
	ImGui::SeparatorText("Collision Debug");	ImGui::Spacing();

	// draw flags inline
	bool drawDyn = debugRenderer->IsFlagEnabled(flags::BVH_Dynamic);
	bool drawSta = debugRenderer->IsFlagEnabled(flags::BVH_Static);
	bool drawFatAABB = debugRenderer->IsFlagEnabled(flags::BVH_FatAABB);
	
	//int  maxDepth = std::max(dynStats.maxDepth, staStats.maxDepth);
	//debugRenderer->SetBVHMaxDepth(std::max(maxDepth, 1));

	if (ImGui::Checkbox("Draw Dynamic", &drawDyn))
		drawDyn ? debugRenderer->SetFlagEnabled(flags::BVH_Dynamic)
		: debugRenderer->SetDisableFlag(flags::BVH_Dynamic);

	ImGui::SameLine();

	if (ImGui::Checkbox("Draw Static", &drawSta))
		drawSta ? debugRenderer->SetFlagEnabled(flags::BVH_Static)
		: debugRenderer->SetDisableFlag(flags::BVH_Static);

	//ImGui::SameLine();
	if (ImGui::Checkbox("Draw Fat AABBs", &drawFatAABB))
		drawFatAABB ? debugRenderer->SetFlagEnabled(flags::BVH_FatAABB)
		: debugRenderer->SetDisableFlag(flags::BVH_FatAABB);

	int maxAllowed = std::max({ dynStats.maxDepth, staStats.maxDepth, 0 });
	static int uiMaxDepth = maxAllowed;

	uiMaxDepth = std::clamp(uiMaxDepth, 0, maxAllowed);

	if (ImGui::SliderInt("Depth", &uiMaxDepth, 0, maxAllowed))
		debugRenderer->SetBVHMaxDepth(uiMaxDepth); //printf("Set max depth to %d, getter returns %d\n", uiMaxDepth, debugRenderer->GetBVHMaxDepth());
	
	debugRenderer->SetBVHMaxDepth(uiMaxDepth);
}


nEntity_ID DebugUIEditor::CreateEntityRand(bool isStatic)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> pos(-50.0f, 50.0f);
	std::uniform_real_distribution<float> Scale(0.5f, 5.0f);
	std::uniform_int_distribution<int> collision(1, 2); 
	std::uniform_real_distribution<float> color(0.0f, 1.0f);
	 
	nNewton::nVector3 tscale(Scale(gen),Scale(gen),Scale(gen));

	int shapeRoll = collision(gen);
	nNewton::nCollisionShapeType shapeType = (shapeRoll == 1)
		? nNewton::nCollisionShapeType::Box
		: nNewton::nCollisionShapeType::Sphere;

	if (shapeType == nNewton::nCollisionShapeType::Sphere)
	{
		tscale = nNewton::nVector3(1);
	}
	auto Transf = nNewton::nTransform({ pos(gen),pos(gen),pos(gen) },
		nNewton::nQuaternion(),
		tscale);

	std::string name;
	name = "Entity" + std::to_string(m_Entities.size());
	
	nNewton::nVector4 Col(color(gen), color(gen), color(gen), 1.0f);
	auto id = CreateEntity(name, 1.0f ,isStatic, shapeType, Transf,nNewton::nVector3(), Col);
	
	return id;
}

nEntity_ID DebugUIEditor::CreateEntity(const std::string& name, float mass, bool isStatic,nNewton::nCollisionShapeType shape,
	const nNewton::nTransform& Transform,const nNewton::nVector3& init_velocity, const nNewton::nVector4& color )
{
	if (!m_RenderSystem) {
		AddLog("Rander Fails System: {}", name);
		return INVALID_ENTITY;
	}

	nNewton::nRigidBodyInfo ShapeInfo;
	ShapeInfo.MASS_ = mass;
	ShapeInfo.INIT_VELOCITY_ = init_velocity;
	ShapeInfo.INIT_TRANSFORM_ = Transform;
	ShapeInfo.IS_STATIC_ = isStatic;

	if (shape == nCollisionShapeType::Box){
		ShapeInfo.SetBoxShape({ 1,1,1 });
	}
	else{
		ShapeInfo.SetSphereShape(1.0f);
	}
	
	nNewton::nEntity_ID shapeID = m_World->Create_Entity(ShapeInfo, true);

	EntityMeta meta;
	meta.id = shapeID;
	meta.name = name.empty() ? "Entity" : name;
	m_Entities.push_back(meta);

	m_RenderSystem->RegisterEntity(shapeID, color);
	
	return shapeID;
}


bool DebugUIEditor::DeleteEntity(nEntity_ID id)
{
	auto remove_it = std::remove_if(m_Entities.begin(), m_Entities.end(),
		[id](const EntityMeta& m) { return m.id == id; });

	if (remove_it == m_Entities.end())
	{
		return false;
	}
	
	m_Entities.erase(remove_it, m_Entities.end());
	m_World->DestroyEntity(id);

	return true;
}

void DebugUIEditor::DestroyAllEntities()
{
	for (auto& n : m_Entities)
	{
		m_World->DestroyEntity(n.id);
	}
	m_Entities.clear();
}

bool DebugUIEditor::RebuildBVHTree(bool isStatic)
{
	
	return m_World->GetCollisionWorld()->RebuildBVH(isStatic);
}

void DebugUIEditor::defaultScene()
{
	// Box1
	CreateEntity(
		"box1",
		1.0f,
		false,
		nNewton::nCollisionShapeType::Box,
		nNewton::nTransform(nNewton::nVector3(0, 1.0f, 0), nNewton::nQuaternion(), nNewton::nVector3(1, 1, 1)),
		nNewton::nVector3(0, 0, 0),
		nNewton::nVector4(0.8f, 0.8f, 0.0f, 1.0f)
	);

	// Box2
	CreateEntity(
		"box2",
		1.0f,
		false,
		nNewton::nCollisionShapeType::Box,
		nNewton::nTransform(nNewton::nVector3(2, 5.0f, 3), nNewton::nQuaternion(), nNewton::nVector3(2, 1, 1)),
		nNewton::nVector3(0, 0, 0),
		nNewton::nVector4(0.6f, 0.8f, 0.3f, 1.0f)
	);

	// Sphere
	CreateEntity(
		"Sphere",
		1.0f,
		false,
		nNewton::nCollisionShapeType::Sphere,
		nNewton::nTransform(nNewton::nVector3(-2, 3, 0), nNewton::nQuaternion(), nNewton::nVector3(1, 1, 1)),
		nNewton::nVector3(0, 0, 0),
		nNewton::nVector4(0.9f, 0.2f, 0.3f, 1.0f)
	);

	// Ground plane
	CreateEntity(
		"ground_plane",
		0.0f,
		true,
		nNewton::nCollisionShapeType::Box,
		nNewton::nTransform(nNewton::nVector3(0, 0, 0), nNewton::nQuaternion(), nNewton::nVector3(5, 0.05f, 5)),
		nNewton::nVector3(0, 0, 0),
		nNewton::nVector4(0.2f, 0.2f, 0.7f, 1.0f)
	);
}

//======== Console panel

DebugConsole::DebugConsole()
{
	ClearLog();
	memset(InputBuf, 0, sizeof(InputBuf));
	HistoryPos = -1;
	RegisterCommands();
	Commands.push_back("help");
	Commands.push_back("history");
	Commands.push_back("clear");
	Commands.push_back("create");
	Commands.push_back("delete");
	Commands.push_back("ncreate");
	Commands.push_back("rebuild-bvh");
	AutoScroll = true;
	ScrollToBottom = false;
	FilterDirty = true;
}

DebugConsole::~DebugConsole()
{
}

void DebugConsole::ClearLog()
{
	Items.clear();
	FilterIndices.clear();
	FilterDirty = true;
}


void DebugConsole::AddLog(char* buf) 
{

	char* start = buf;
	char* end = strchr(start, '\n');



	while (end != nullptr) {
		*end = '\0';   

		LogEntry entry;
		entry.Text = start;
		entry.HasColor = false;

		if (entry.Text.find("[Error]") != std::string::npos) {
			entry.Color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
			entry.HasColor = true;
		}
		else if (entry.Text.starts_with("> ")) {
			entry.Color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f);
			entry.HasColor = true;
		}
		else if (entry.Text.find("[Success]") != std::string::npos)
		{
			entry.Color = ImVec4(0.2f, 0.8f, 0.5f, 1.0f);
			entry.HasColor = true;
		}

		Items.push_back(entry);
		start = end + 1;         
		end = strchr(start, '\n'); 
	}

	if (*start != '\0') {

		LogEntry entry;
		entry.Text = start;
		entry.HasColor = false;

		if (entry.Text.find("[Error]") != std::string::npos) {
			entry.Color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
			entry.HasColor = true;
		}
		else if (entry.Text.starts_with("> ")) {
			entry.Color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f);
			entry.HasColor = true;
		}
		else if (entry.Text.find("[Success]") != std::string::npos)
		{
			entry.Color = ImVec4(0.2f, 0.8f, 0.5f, 1.0f);
			entry.HasColor = true;
		}
		Items.push_back(entry);
	}

	FilterDirty = true;
}

void DebugConsole::Draw(const char* title, bool* p_open)
{
	ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin(title, p_open))
	{
		ImGui::End();
		return;
	}

	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::MenuItem("Close Console"))
			*p_open = false;
		ImGui::EndPopup();
	}


	ImGui::TextWrapped("Enter 'HELP' for help.");

	// TODO: display items starting from the bottom

	if (ImGui::SmallButton("Clear")) { ClearLog(); }
	ImGui::SameLine();
	bool copy_to_clipboard = ImGui::SmallButton("Copy");

	//ImGui::Separator();
	ImGui::Spacing();
	// Options menu
	if (ImGui::BeginPopup("Options"))
	{
		ImGui::Checkbox("Auto-scroll", &AutoScroll);
		ImGui::EndPopup();
	}

	// Options, Filter
	ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_O, ImGuiInputFlags_Tooltip);
	if (ImGui::Button("Options"))
		ImGui::OpenPopup("Options");
	ImGui::SameLine();


	static char search_buf[128] = "";

	//ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
	ImGui::SetNextItemWidth(250.0f); 

	if (ImGui::InputTextWithHint("##ConsoleSearch", "Search logs...", search_buf, IM_ARRAYSIZE(search_buf)))
	{
		strcpy(Filter.InputBuf, search_buf);

		Filter.Build();

		FilterDirty = true;
	}

	//ImGui::PopStyleVar();
	if (search_buf[0] != '\0') {
		ImGui::SameLine();
		if (ImGui::Button(" X ")) {
			search_buf[0] = '\0';
			Filter.InputBuf[0] = '\0';
			Filter.Build();
			FilterDirty = true;
		}
	}

	ImGui::Spacing();
	//ImGui::Separator();

	if (FilterDirty) {
		UpdateFilter();
	}

	// Reserve enough left-over height for 1 separator + 1 input text
	ImGuiStyle& style = ImGui::GetStyle();
	const float footer_height_to_reserve = style.SeparatorSize + style.ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
	if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_HorizontalScrollbar))
	{
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::Selectable("Clear")) ClearLog();
			ImGui::EndPopup();
		}

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
		if (copy_to_clipboard)
			ImGui::LogToClipboard();
		
		ImGuiListClipper clip;
		clip.Begin(FilterIndices.size());

		while (clip.Step())
		{
			for (int i = clip.DisplayStart; i < clip.DisplayEnd; ++i)
			{
				int realIndex = FilterIndices[i];
				const LogEntry& item = Items[realIndex]; 
				if (item.HasColor)
					ImGui::PushStyleColor(ImGuiCol_Text, item.Color);

				ImGui::TextUnformatted(item.Text.c_str());

				if (item.HasColor)
					ImGui::PopStyleColor();
			}
		}
		
		if (copy_to_clipboard)
			ImGui::LogFinish();

		// Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
		// Using a scrollbar or mouse-wheel will take away from the bottom edge.
		if (ScrollToBottom || (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
			ImGui::SetScrollHereY(1.0f);
		ScrollToBottom = false;

		ImGui::PopStyleVar();
	}
	ImGui::EndChild();
	//ImGui::Separator();

	// Command-line
	bool reclaim_focus = false;
	ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory ;
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 5));
	ImGui::PushItemWidth(-1.0f);
	if (ImGui::InputTextWithHint("##Input", "Type command here...", InputBuf, IM_COUNTOF(InputBuf), input_text_flags,
		[](ImGuiInputTextCallbackData* data) { DebugConsole* console = (DebugConsole*)data->UserData;
	return console->TextEditCallback(data); }, (void*)this))
	{
		std::string s = InputBuf;

		if (!s.empty())
		{
			std::vector<std::string> tokens;
			std::stringstream ss(s);
			std::string temp;

			while (ss >> temp) {
				tokens.push_back(temp);
			}

			//std::string cmdName = tokens[0];

			ExecCommand(tokens);
		}
		InputBuf[0] = '\0';             
		reclaim_focus = true;
	}
	ImGui::PopItemWidth();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
	// Auto-focus on window apparition
	ImGui::SetItemDefaultFocus();
	if (reclaim_focus)
		ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

	ImGui::End();
}

void    DebugConsole::ExecCommand(const std::vector<std::string>& command_line)
{
	size_t total_size = 0;
	for (const auto& s : command_line) total_size += s.size() + 1;

	std::string full_command_line;
	full_command_line.reserve(total_size); 

	for (size_t i = 0; i < command_line.size(); ++i) {
		full_command_line += command_line[i];
		if (i < command_line.size() - 1) full_command_line += " ";
	}


	DebugUIEditor::AddLog("> {}\n", full_command_line.c_str());
	
	HistoryPos = -1;
	History.erase(std::remove(History.begin(), History.end(), full_command_line), History.end());
	History.push_back(full_command_line);

	std::string cmd = command_line[0];	
	std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);

	auto it = m_CommandMap.find(cmd);
	if (it != m_CommandMap.end())
	{
		it->second(command_line);
	}
	else
	{
		DebugUIEditor::AddLog("Unknown command: '{}'\n", cmd.c_str());
	}

	// On command input, we scroll to bottom even if AutoScroll==false
	ScrollToBottom = true;
}

int DebugConsole::TextEditCallback(ImGuiInputTextCallbackData* data)
{
	//AddLog("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
	switch (data->EventFlag)
	{
	case ImGuiInputTextFlags_CallbackCompletion:
	{
		// Locate beginning of current word
		const char* word_end = data->Buf + data->CursorPos;
		const char* word_start = word_end;
		while (word_start > data->Buf)
		{
			const char c = word_start[-1];
			if (c == ' ' || c == '\t' || c == ',' || c == ';')
				break;
			word_start--;
		}

		// Build a list of candidates
		ImVector<const char*> candidates;
		for (int i = 0; i < Commands.size(); i++)
			if (CaseInsensitiveMatchStart(std::string_view(Commands[i]), std::string_view(word_start, (int)(word_end - word_start))))
				candidates.push_back(Commands[i]);

		if (candidates.Size == 0)
		{
			// No match
			DebugUIEditor::AddLog("No match for \"{:.{}}\"!\n", word_start ,(int)(word_end - word_start));
		}
		else if (candidates.Size == 1)
		{
			// Single match. Delete the beginning of the word and replace it entirely so we've got nice casing.
			data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
			data->InsertChars(data->CursorPos, candidates[0]);
			data->InsertChars(data->CursorPos, " ");
		}
		else
		{
			// Multiple matches. Complete as much as we can..
			// So inputting "C"+Tab will complete to "CL" then display "CLEAR" and "CLASSIFY" as matches.
			int match_len = (int)(word_end - word_start);
			for (;;)
			{
				int c = 0;
				bool all_candidates_matches = true;
				for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
					if (i == 0)
						c = toupper(candidates[i][match_len]);
					else if (c == 0 || c != toupper(candidates[i][match_len]))
						all_candidates_matches = false;
				if (!all_candidates_matches)
					break;
				match_len++;
			}

			if (match_len > 0)
			{
				data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
				data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
			}

			// List matches
			DebugUIEditor::AddLog("\nPossible matches:\n");
			for (int i = 0; i < candidates.Size; i++)
				DebugUIEditor::AddLog("- {}\n", candidates[i]);
		}

		break;
	}
	case ImGuiInputTextFlags_CallbackHistory:
	{
		// HISTORY
		const int prev_history_pos = HistoryPos;
		if (data->EventKey == ImGuiKey_UpArrow)
		{
			if (HistoryPos == -1)
				HistoryPos = History.size() - 1;
			else if (HistoryPos > 0)
				HistoryPos--;
		}
		else if (data->EventKey == ImGuiKey_DownArrow)
		{
			if (HistoryPos != -1)
				if (++HistoryPos >= History.size())
					HistoryPos = -1;
		}

		// A better implementation would preserve the data on the current input line along with cursor position.
		if (prev_history_pos != HistoryPos)
		{
			const char* history_str = (HistoryPos >= 0) ? History[HistoryPos].c_str() : "";
			data->DeleteChars(0, data->BufTextLen);
			data->InsertChars(0, history_str);
		}
	}
	}
	return 0;
}

void DebugConsole::RegisterCommands()
{
	m_CommandMap["CLEAR"] = [this](const std::vector<std::string>& command_line) {
		Items.clear();
		};

	m_CommandMap["HELP"] = [this](const std::vector<std::string>& command_line) {
		DebugUIEditor::AddLog("Commands:");
		for (const auto& cmd : Commands)
			DebugUIEditor::AddLog("- {}", cmd);
		};

	//CREATE CMD
	m_CommandMap["CREATE"] = [this](const std::vector<std::string>& command_line) {
		if (command_line.size() < 4) {
			DebugUIEditor::AddLog("[Error] Usage: CREATE <DYNAMIC/STATIC> <BOX|SPHERE> <NAME>");
			return;
		}
		bool isStatic = CaseInsensitiveMatch(command_line[1], "STATIC");
		bool isDynamic = CaseInsensitiveMatch(command_line[1], "DYNAMIC");

		if (!isStatic && !isDynamic) {
			DebugUIEditor::AddLog("[Error] Invalid type '{}'. Use STATIC or DYNAMIC.", command_line[1]);
			return;
		}

		nNewton::nCollisionShapeType shapeType;
		if (CaseInsensitiveMatch(command_line[2], "BOX")) {
			shapeType = nCollisionShapeType::Box;
		}
		else if (CaseInsensitiveMatch(command_line[2], "SPHERE")) {
			shapeType = nCollisionShapeType::Sphere;
		}
		else {
			DebugUIEditor::AddLog("[Error] Unknown shape '{}'. Supported: BOX, SPHERE.", command_line[2]);
			return;
		}

		float mass = isStatic ? 0.0f : 1.0f;
		m_Owner->CreateEntity(command_line[3], mass, isStatic, shapeType);

		DebugUIEditor::AddLog("[Success] Created {} {} named {}", command_line[1], command_line[2], command_line[3]);
	};

	//nCREATE CMD
	m_CommandMap["NCREATE"] = [this](const std::vector<std::string> command_line) {
		if (command_line.size() < 3)
		{
			DebugUIEditor::AddLog("[Error] Usage: CREATE <DYANMIC/STATIC> <COUNT>");
			return;
		}

		bool isStatic = CaseInsensitiveMatch(command_line[1], "STATIC");
		bool isDynamic = CaseInsensitiveMatch(command_line[1], "DYNAMIC");

		if (!isStatic && !isDynamic) {
			DebugUIEditor::AddLog("[Error] Invalid type '{}'. Use STATIC or DYNAMIC.", command_line[1]);
			return;
		}

		auto& input = command_line[2];

		if (input[1] == '-')
		{
			DebugUIEditor::AddLog("[Error] Invalid count");
			return;
		}

		uint16_t count;
		auto [ptr, err] = std::from_chars(input.data(), input.data() + input.size(), count);

		if (err == std::errc::invalid_argument)
		{
			DebugUIEditor::AddLog("[Error] Invalid count");
			return;
		}
		else if (err == std::errc::result_out_of_range)
		{
			DebugUIEditor::AddLog("[Error] Invalid count");
			return;
		}
		else if (ptr != input.data() + input.size())
		{
			DebugUIEditor::AddLog("[Error] Invalid count");
			return;
		}
		else
		{
			DebugUIEditor::AddLog("[Log] Started Creating {} Random Entity ", count);
			for (size_t i = 0; i < count; i++)
			{
				auto id = m_Owner->CreateEntityRand(isStatic);
				DebugUIEditor::AddLog("[Success] Created Entity ID : {}", id);
			}
			DebugUIEditor::AddLog("[Info] Created {} Random Entity ", count);
		}
	};

	m_CommandMap["DELETE"] = [this](const std::vector<std::string> command_line) {
		if (command_line.size() < 2)
		{
			DebugUIEditor::AddLog("[Error] Usage: DELETE <ID>");
			return;
		}

		std::string input = command_line[1];
		if (CaseInsensitiveMatch(input, "ALL"))
		{
			m_Owner->DestroyAllEntities();
			DebugUIEditor::AddLog("[Success] Entities deleted ");
			return;
		}

		if (input[1] == '-')
		{
			DebugUIEditor::AddLog("[Error] Invalid ID");
			return;
		}


		uint32_t id;
		auto [ptr, err] = std::from_chars(input.data(), input.data() + input.size(), id);

		if (err == std::errc::invalid_argument)
		{
			DebugUIEditor::AddLog("[Error] Invalid ID");
			return;
		}
		else if (err == std::errc::result_out_of_range)
		{
			DebugUIEditor::AddLog("[Error] Invalid ID");
			return;
		}
		else if (ptr != input.data() + input.size())
		{
			DebugUIEditor::AddLog("[Error] Invalid ID");
			return;
		}
		else
		{
			if (!m_Owner->DeleteEntity(id)) {
				DebugUIEditor::AddLog("[Error] No entity found with ID: {}", id);
				return;
			}

			DebugUIEditor::AddLog("[Success] Entity {} deleted.", id);
		}
	};

	m_CommandMap["REBUILD-BVH"] = [this](const std::vector<std::string> command_line) {

		if (command_line.size() < 2)
		{
			DebugUIEditor::AddLog("[Error] Usage: rebuild-bvh <static/dynamic>");
			return;
		}

		bool isStatic = CaseInsensitiveMatch(command_line[1], "STATIC");
		bool isDynamic = CaseInsensitiveMatch(command_line[1], "DYNAMIC");

		if (!isStatic && !isDynamic) {
			DebugUIEditor::AddLog("[Error] Invalid type '{}'. Use STATIC or DYNAMIC.", command_line[1]);
			return;
		}
		
		 if(m_Owner->RebuildBVHTree(isStatic)){
			 DebugUIEditor::AddLog("{}", isStatic ? "[Success] Static BVH Tree Rebuild." : "[Success] Dynamic BVH Tree Rebuild.");
			 return;
		 }
		 
		 DebugUIEditor::AddLog("[Error] Failed to Rebuild {} Tree.", isStatic ? "Static" : "Dynamic");

		};
}

static std::string Strtrim(const std::string& str) {
	const std::string whitespace = " \t\r\n\v\f";

	size_t start = str.find_first_not_of(whitespace);
	if (start == std::string::npos) return "";

	size_t end = str.find_last_not_of(whitespace);

	return str.substr(start, end - start + 1);
}

static bool CaseInsensitiveMatch(std::string_view str1, std::string_view str2) {
	if (str1.size() != str2.size()) return false;
	return std::equal(str1.begin(), str1.end(), str2.begin(),
		[](unsigned char c1, unsigned char c2) {
			return std::tolower(c1) == std::tolower(c2);
		});
}

static bool CaseInsensitiveMatchStart(std::string_view str, std::string_view pref) {
	if (pref.size() > str.size()) return false;
	return std::equal(pref.begin(), pref.end(), str.begin(),
		[](unsigned char c1, unsigned char c2) {
			return std::tolower(c1) == std::tolower(c2);
		});
}

void DebugUIEditor::DrawConsole(bool* p_open)
{
	GetConsole().Draw(" Console ", p_open);
}

template<typename... Args>
void DebugUIEditor::AddLog(std::format_string<Args...> fmt, Args&&... args )
{

	static DebugConsole console;

	std::string massage = std::format(fmt, std::forward<Args>(args)...);
	

	GetConsole().AddLog(massage.data());
}
