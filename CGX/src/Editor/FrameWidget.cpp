#include "FrameWidget.h"

#include "../ImGUI/imgui_impl_opengl3.h"
#include "../ImGUI/imgui_impl_glfw.h"

void FrameWidget::Initialize()
{

}

void FrameWidget::Update(float deltatime)
{
	ImGui::Begin("Frame Rate");
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();
}

void FrameWidget::ShutDown()
{

}
