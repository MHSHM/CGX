#include "Editor.h"
#include "../Threading/ThreadPool.h"
#include "Game.h"
#include "Scene.h"
#include "FrameWidget.h"
#include "SceneHierarchyWidget.h"

#include "../ImGUI/imgui_impl_opengl3.h"
#include "../ImGUI/imgui_impl_glfw.h"

#include <future>

bool Editor::Initialize(Game* _game)
{
	game = _game;

	m_widgets.reserve(MAX_WIDGETS); 

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(game->window.window_ptr, true);
	ImGui_ImplOpenGL3_Init("#version 440");

	return true; 
}

void Editor::Setup(Scene* scene)
{
	// Frame Rate
	m_widgets.push_back(new FrameWidget);

	// Scene Hierarchy
	m_widgets.push_back(new SceneHierarchyWidget(scene->nodes_ptrs));
}

void Editor::Update(float deltatime)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	for (auto& widget : m_widgets)
	{
		widget->Update(deltatime);
	}
}

void Editor::Draw()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Editor::ShutDown()
{
	for (auto& widget : m_widgets) 
	{
		widget->ShutDown(); 
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
