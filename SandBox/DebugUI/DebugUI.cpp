#include "DebugUI.hpp"



bool DebugUIEditor::Init_DebugUIEditor(Window* window, std::shared_ptr<DebugRenderer> render, nNewton::nDynamicsWorld* world,
	nRenderSystem* renderSystem)
{
	SDL_Window = window;
	debugRenderer = render;
	m_World = world;
	std::cout << "debug render this=" << debugRenderer.get() << std::endl;
	m_RenderSystem = renderSystem;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsClassic();
	ApplyCustomStyle();
	ImGuiIO& io = ImGui::GetIO();

	io.Fonts->AddFontFromFileTTF("assets/Fonts/JetBrainsMonoNL-Regular.ttf", 13.0f);
	//io.DisplaySize = ImVec2((float)SDL_Window->getWidth(), (float)SDL_Window->getHeight());

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;



	ImGui_ImplSDL3_InitForOpenGL(SDL_Window->GetNativeHandle(), SDL_Window->GetSDLglContext());
	ImGui_ImplOpenGL3_Init("#version 460");

	return true;
}

void DebugUIEditor::ApplyCustomStyle()
{
	ImGuiStyle& s = ImGui::GetStyle();

	//rounding
	s.WindowRounding = 4.f;
	s.FrameRounding = 4.f;
	s.PopupRounding = 4.f;
	s.ScrollbarRounding = 4.f;
	s.GrabRounding = 4.f;
	s.TabRounding = 4.f;
	s.ChildRounding = 4.f;

	//spacing
	s.WindowPadding = ImVec2(10, 10);
	s.FramePadding = ImVec2(4, 4);
	s.ItemSpacing = ImVec2(8, 6);
	s.ItemInnerSpacing = ImVec2(6, 4);
	s.IndentSpacing = 14.f;
	s.ScrollbarSize = 10.f;
	s.GrabMinSize = 10.f;
	s.WindowBorderSize = 2.f;
	s.FrameBorderSize = 0.f;
	s.PopupBorderSize = 1.f;

	ImVec4* c = s.Colors;

	// base
	c[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.13f, 0.14f, 1.00f);
	c[ImGuiCol_ChildBg] = ImVec4(0.10f, 0.10f, 0.11f, 1.00f);
	c[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.13f, 0.14f, 1.00f);
	c[ImGuiCol_Border] = ImVec4(0.30f, 0.30f, 0.32f, 0.60f);

	// text
	c[ImGuiCol_Text] = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
	c[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.52f, 1.00f);

	// headers (CollapsingHeader, TreeNode, Selectable)
	c[ImGuiCol_Header] = ImVec4(0.26f, 0.26f, 0.28f, 1.00f);
	c[ImGuiCol_HeaderHovered] = ImVec4(0.32f, 0.32f, 0.35f, 1.00f);
	c[ImGuiCol_HeaderActive] = ImVec4(0.22f, 0.22f, 0.25f, 1.00f);

	// frames (InputText, Combo, Slider bg)
	c[ImGuiCol_FrameBg] = ImVec4(0.18f, 0.18f, 0.20f, 1.00f);
	c[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.24f, 0.26f, 1.00f);
	c[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);

	// accent 
	ImVec4 accent = ImVec4(0.95f, 0.15f, 0.15f, 1.00f);
	ImVec4 accentHover = ImVec4(0.36f, 0.64f, 1.00f, 1.00f);
	ImVec4 accentClick = ImVec4(0.20f, 0.48f, 0.80f, 1.00f);

	c[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.16f, 0.18f, 1.00f);
	c[ImGuiCol_TitleBg] = ImVec4(0.12f, 0.12f, 0.13f, 1.00f);
	c[ImGuiCol_TitleBgCollapsed] = ImVec4(0.12f, 0.12f, 0.13f, 0.80f);
	c[ImGuiCol_MenuBarBg] = ImVec4(0.16f, 0.16f, 0.18f, 1.00f);
	c[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.10f, 0.11f, 1.00f);
	c[ImGuiCol_ScrollbarGrab] = ImVec4(0.30f, 0.30f, 0.32f, 1.00f);
	c[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.42f, 1.00f);
	c[ImGuiCol_ScrollbarGrabActive] = accent;

	c[ImGuiCol_CheckMark] = accent;
	c[ImGuiCol_SliderGrab] = accent;
	c[ImGuiCol_SliderGrabActive] = accentClick;

	c[ImGuiCol_Button] = ImVec4(0.22f, 0.22f, 0.25f, 1.00f);
	c[ImGuiCol_ButtonHovered] = accentHover;
	c[ImGuiCol_ButtonActive] = accentClick;

	c[ImGuiCol_Tab] = ImVec4(0.16f, 0.16f, 0.18f, 1.00f);
	c[ImGuiCol_TabHovered] = accentHover;
	c[ImGuiCol_TabActive] = accent;
	c[ImGuiCol_TabUnfocused] = ImVec4(0.14f, 0.14f, 0.16f, 1.00f);
	c[ImGuiCol_TabUnfocusedActive] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);

	c[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.30f, 1.00f);
	c[ImGuiCol_ResizeGrip] = ImVec4(0.30f, 0.30f, 0.32f, 0.50f);
	c[ImGuiCol_ResizeGripHovered] = accentHover;
	c[ImGuiCol_ResizeGripActive] = accent;
}




void DebugUIEditor::BeginUIFrame()
{

	ImGui_ImplOpenGL3_NewFrame();

	ImGui_ImplSDL3_NewFrame();

	ImGui::NewFrame();

}
void DebugUIEditor::EndUIFrame()
{
	ImGui::Render();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}

void DebugUIEditor::ShutDownUI()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();
}

void DebugUIEditor::RenderUI(bool* IsOverlay, bool* IsPanels )
{
	Stats_Overlay(IsOverlay);
	DrawEntityListPanel(IsPanels);
	DrawInspectorPanel(IsPanels);
}


void DebugUIEditor::Stats_Overlay(bool* IsOverlay)
{
	static int location = 0;
	ImGuiIO& io = ImGui::GetIO();
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
	if (location >= 0)
	{
		const float	PAD = 10.0f;
		const ImGuiViewport* viewport = ImGui::GetMainViewport();

		ImVec2 work_pos = viewport->WorkPos;
		ImVec2 work_size = viewport->WorkSize;
		ImVec2 Window_POS, Window_POS_Pivot;

		Window_POS.x = (location & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
		Window_POS.y = (location & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
		Window_POS_Pivot.x = (location & 1) ? 1.0f : 0.0f;
		Window_POS_Pivot.y = (location & 2) ? 1.0f : 0.0f;
		ImGui::SetNextWindowPos(Window_POS, ImGuiCond_Always, Window_POS_Pivot);

		//UI WINDOW SIZE 
		ImGui::SetNextWindowSize(ImVec2(450.0f, 350.0f), ImGuiCond_Always);
		window_flags |= ImGuiWindowFlags_NoMove;

	}


	ImGui::SetNextWindowBgAlpha(0.2f);
	ImGui::Begin("Stats Overlay", IsOverlay, window_flags);
	//ImGui::SetWindowFontScale(1.5f);
	io.FontGlobalScale = 2.0f;
	ImGui::Text("nNewton Statistics:\n""Mayank Bairagi");
	ImGui::Separator();

	ImGui::Text("FrameRate : %.1f FPS\nFrameTime : %.1f MS", io.Framerate, 1000 / io.Framerate);

	ImGui::Separator();

	if (ImGui::IsMousePosValid())
		ImGui::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
	else
		ImGui::Text("Mouse Position: <invalid>");

	ImGui::Separator();

	ImGui::Text("Numbers of Objects: %s\n", "NA");
	int vertCount = debugRenderer->GetLineCount() * 2;
	//printf("Stats_Overlay lineCount = %d\n", lineCount);
	ImGui::Text("Numbers of Vertex: %d\n", vertCount);

	ImGui::Separator();

	ImGui::Text("Numbers of Collision: %s\n", "NA");

	ImGui::Separator();
	int w, h;
	SDL_GetWindowSize(SDL_Window->GetNativeHandle(), &w, &h);
	ImGui::Text("Window Size: %d X %d\n", w, h);
	ImGui::End();
	
}



void DebugUIEditor::DrawBVHStats()
{
	nNewton::nBVHStats staticStats = m_World->GetCollisionWorld()->GetStaticTree()->CollectStats();
	nNewton::nBVHStats dynamicStats = m_World->GetCollisionWorld()->GetDynamicTree()->CollectStats();

	bool drawStatic = debugRenderer->IsFlagEnabled(flags::BVH_Static);
	bool drawDynamic = debugRenderer->IsFlagEnabled(flags::BVH_Dynamic);
	bool drawFatAABB = debugRenderer->IsFlagEnabled(flags::BVH_FatAABB);
	int  maxDepth = debugRenderer->GetBVHMaxDepth();

	ImGui::Begin("BVH Debug");

	if (ImGui::CollapsingHeader("Static Tree")) {
		ImGui::Text("Total Nodes  : %d", staticStats.totalNodes);
		ImGui::Text("Leaf Nodes   : %d", staticStats.leafNodes);
		ImGui::Text("Max Depth    : %d", staticStats.maxDepth);
		ImGui::Text("Avg Depth    : %.2f", staticStats.avgDepth);
	}

	if (ImGui::CollapsingHeader("Dynamic Tree")) {
		ImGui::Text("Total Nodes  : %d", dynamicStats.totalNodes);
		ImGui::Text("Leaf Nodes   : %d", dynamicStats.leafNodes);
		ImGui::Text("Dirty Nodes  : %d", dynamicStats.dirtyNodes);
		ImGui::Text("Max Depth    : %d", dynamicStats.maxDepth);
		ImGui::Text("Avg Depth    : %.2f", dynamicStats.avgDepth);
		ImGui::Text("Queue Size   : %d", dynamicStats.queueSize);
	}

	ImGui::Separator();

	if (ImGui::Checkbox("Draw Static Tree", &drawStatic))
		drawStatic ? debugRenderer->SetFlagEnabled(flags::BVH_Static)
		: debugRenderer->SetDisableFlag(flags::BVH_Static);

	if (ImGui::Checkbox("Draw Dynamic Tree", &drawDynamic))
		drawDynamic ? debugRenderer->SetFlagEnabled(flags::BVH_Dynamic)
		: debugRenderer->SetDisableFlag(flags::BVH_Dynamic);

	if (ImGui::Checkbox("Draw Fat AABBs", &drawFatAABB))
		drawFatAABB ? debugRenderer->SetFlagEnabled(flags::BVH_FatAABB)
		: debugRenderer->SetDisableFlag(flags::BVH_FatAABB);

	int maxAllowed = std::max(staticStats.maxDepth, dynamicStats.maxDepth);
	if (ImGui::SliderInt("Max Draw Depth", &maxDepth, 1, maxAllowed))
		debugRenderer->SetBVHMaxDepth(maxDepth);

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
	float buttonWidth = ImGui::CalcTextSize(" + ").x + ImGui::GetStyle().FramePadding.x * 2;
	
	ImGui::SameLine(ImGui::GetWindowContentRegionMax().x  - buttonWidth * 2 - 8.f);

	if (ImGui::SmallButton(" + "))
		ImGui::OpenPopup("AddEntityPopup");
	if (ImGui::IsItemHovered()) ImGui::SetTooltip("Add entity");

	ImGui::SameLine(0, 4);

	bool hasSelection = m_World && m_World->IsValid(m_SelectedID);
	if (!hasSelection) ImGui::BeginDisabled();
	if (ImGui::SmallButton(" - ")) {
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
		ImGui::Text("@");
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
		info.INIT_TRANSFORM_.SetScale({ m_NewHalfExt[0], m_NewHalfExt[1], m_NewHalfExt[2] });
		info.INIT_TRANSFORM_.SetRotation(nQuaternion{ 0.f, 0.f, 0.f, 1.f });

		info.SetBoxShape({ 1.f, 1.f, 1.f });

		RenderObjectType renderType = RenderObjectType::DEBUG_BOX;
		

		switch (m_NewShapeType) {
		case 0:
			
			renderType = RenderObjectType::DEBUG_BOX;
			break;
		case 1:
			renderType = RenderObjectType::DEBUG_SPHERE;
			break;
		}

		nNewton::nEntity_ID newID = m_World->Create_Entity(info,true);

		

		//m_World->GetCollisionWorld()->CreateCollisionEntity(newID, info.IS_STATIC_, info.INIT_TRANSFORM_, info.INIT_VELOCITY_, info.getCollisionShape());

		if (m_RenderSystem)
			m_RenderSystem->RegisterEntity(newID, renderType, m_NewColor);


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
		m_NewIsStatic = false;

		ImGui::CloseCurrentPopup();
	}

	ImGui::SameLine();
	if (ImGui::Button("Cancel", ImVec2(80, 0)))
		ImGui::CloseCurrentPopup();

	

	ImGui::EndPopup();

}


void DebugUIEditor::DrawInspectorPanel(bool* open)
{
	if (!ImGui::Begin("Inspector Panel", open)) {
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

	DrawBVHStatsInline();
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
	if (!ImGui::CollapsingHeader("BVH Debug"))
		return;

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

	ImGui::Separator();

	// draw flags inline
	bool drawDyn = debugRenderer->IsFlagEnabled(flags::BVH_Dynamic);
	bool drawSta = debugRenderer->IsFlagEnabled(flags::BVH_Static);
	int  maxDepth = debugRenderer->GetBVHMaxDepth();

	if (ImGui::Checkbox("Draw Dynamic", &drawDyn))
		drawDyn ? debugRenderer->SetFlagEnabled(flags::BVH_Dynamic)
		: debugRenderer->SetDisableFlag(flags::BVH_Dynamic);

	ImGui::SameLine();

	if (ImGui::Checkbox("Draw Static", &drawSta))
		drawSta ? debugRenderer->SetFlagEnabled(flags::BVH_Static)
		: debugRenderer->SetDisableFlag(flags::BVH_Static);

	int maxAllowed = std::max(dynStats.maxDepth, staStats.maxDepth);
	maxAllowed = std::max(maxAllowed, 1);
	if (ImGui::SliderInt("Max Depth", &maxDepth, 1, maxAllowed))
		debugRenderer->SetBVHMaxDepth(maxDepth);
}