#include "DebugUI.hpp"



bool DebugUIEditor::Init_DebugUIEditor(Window* window, std::shared_ptr<DebugRenderer> render, nNewton::nCollisionWorld* collisionW)
{
	SDL_Window = window;
	debugRenderer = render;
	m_CollisionWorld = collisionW;
	std::cout << "debug render this=" << debugRenderer.get() << std::endl;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	//io.DisplaySize = ImVec2((float)SDL_Window->getWidth(), (float)SDL_Window->getHeight());

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;



	ImGui_ImplSDL3_InitForOpenGL(SDL_Window->GetNativeHandle(), SDL_Window->GetSDLglContext());
	ImGui_ImplOpenGL3_Init("#version 460");

	return true;
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
	DrawBVHStats();
}



void DebugUIEditor::DrawBVHStats() {

	nNewton::nBVHStats staticStats = m_CollisionWorld->GetStaticTree()->CollectStats();
	nNewton::nBVHStats dynamicStats = m_CollisionWorld->GetDynamicTree()->CollectStats();

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
	ImGui::Checkbox("Draw Static Tree", &m_DrawStatic);
	ImGui::Checkbox("Draw Dynamic Tree", &m_DrawDynamic);
	ImGui::Checkbox("Draw Fat AABBs", &m_DrawFatAABB);
	ImGui::SliderInt("Max Draw Depth", &m_DebugMaxDepth, 1,
		std::max(staticStats.maxDepth, dynamicStats.maxDepth));

	ImGui::End();
}


