#include "SceneHierarchyWidget.h"
#include "SceneNode.h"
#include "Actor.h"
#include "Component.h"
#include "Transform.h"
#include "SkyBox.h"
#include "QuadLight.h"
#include "PointLight.h"
#include "Model.h"
#include "Texture.h"
#include "LDRTexture.h"
#include "Camera.h"

#include "../ImGUI/imgui_impl_opengl3.h"
#include "../ImGUI/imgui_impl_glfw.h"

#include <iostream>

SceneHierarchyWidget::SceneHierarchyWidget(std::vector<class SceneNode*>& nodes):
	m_scene_nodes(nodes)
{
}

void SceneHierarchyWidget::Initialize()
{

}

void SceneHierarchyWidget::Update(float deltatime)
{
	ImGui::Begin("Scene Hierarchy");

	for (auto& node : m_scene_nodes)
	{
		if (node->children.size() > 0)
		{
			Create_Parent_Child_Hierarchy(node, deltatime);
		}
		else 
		{
			if (node->parent)
			{
				continue;
			}
			
			if (ImGui::TreeNode(node->tag.c_str()))
			{
				Display_Components_Window(node, deltatime);

				ImGui::TreePop();
			}
		}
	}

	ImGui::End();
}

void SceneHierarchyWidget::ShutDown()
{
	m_scene_nodes.clear();
}

void SceneHierarchyWidget::Create_Parent_Child_Hierarchy(SceneNode* node, float deltatime)
{
	if (node->children.size() > 0) 
	{
		if (ImGui::TreeNode(node->tag.c_str())) 
		{
			for (auto& child : node->children)
			{
				Create_Parent_Child_Hierarchy(child, deltatime);
			}

			ImGui::TreePop();
		}

		return; 
	}

	if (ImGui::TreeNode(node->tag.c_str())) 
	{
		Display_Components_Window(node, deltatime);

		ImGui::TreePop();
	}
}

void SceneHierarchyWidget::Display_Components_Window(SceneNode* node, float deltatime)
{
	Transform* transform = node->actor->Get_Component<Transform>(); 
	SkyBox* skybox = node->actor->Get_Component<SkyBox>(); 
	QuadLight* quadlight = node->actor->Get_Component<QuadLight>(); 
	PointLight* pointlight = node->actor->Get_Component<PointLight>();
	Model* model = node->actor->Get_Component<Model>();
	Camera* camera = node->actor->Get_Component<Camera>(); 

	if (transform) Create_Transform_Window(transform, deltatime);
	if (camera) Create_Camera_Window(camera, deltatime);
	if (model) Create_Model_Window(model);
	if (skybox) Create_Skybox_Window(skybox);
	if (quadlight) Create_Quadlight_Window(quadlight, deltatime);
	if (pointlight) Create_Pointlight_Window(pointlight);
}

void SceneHierarchyWidget::Create_Transform_Window(Transform* transform, float deltatime)
{
	const float MAX = 100.0f;
	const float MIN = -100.0f;
	const float transslation_speed = 1.0f;
	const float scale_speed = 0.1f;
	const float rotation_speed = 1.1f;

	float* rotation_angles[3] = { &transform->angle_x, &transform->angle_y, &transform->angle_z };

	ImGui::Separator();
	ImGui::TextColored(ImVec4(0.0f, 0.9f, 0.2f, 1.0f), "Transform");
	ImGui::DragFloat3("translation", &transform->translation.x, transslation_speed * deltatime, MIN, MAX);
	ImGui::DragFloat3("rotation", rotation_angles[0], rotation_speed * deltatime, MIN, MAX);
	ImGui::DragFloat("scale", &transform->scale, scale_speed * deltatime, MIN, MAX);
	ImGui::Separator(); 
}

void SceneHierarchyWidget::Create_Skybox_Window(SkyBox* skybox)
{
	ImGui::Text("Skybox");
	ImGui::Separator();
}

void SceneHierarchyWidget::Create_Quadlight_Window(QuadLight* quadlight, float deltatime)
{
	const float MAX = 100.0f;
	const float MIN = 0.0f;

	const float speed = 5.0f; 

	ImGui::TextColored(ImVec4(0.0f, 0.9f, 0.2f, 1.0f), "Quadlight");
	ImGui::DragFloat3("intensity", &quadlight->intensity.x, speed * deltatime, 0.0f, 100.0f);
	ImGui::DragFloat3("vertex 0", &quadlight->vertices[0].x, 0.0f);
	ImGui::DragFloat3("vertex 1", &quadlight->vertices[1].x, 0.0f);
	ImGui::DragFloat3("vertex 2", &quadlight->vertices[2].x, 0.0f);
	ImGui::DragFloat3("vertex 3", &quadlight->vertices[3].x, 0.0f);
	ImGui::Separator();
}

void SceneHierarchyWidget::Create_Pointlight_Window(PointLight* pointlight)
{
	ImGui::Text("Pointlight");
	ImGui::Separator();
}

void SceneHierarchyWidget::Create_Model_Window(Model* model)
{
	ImGui::TextColored(ImVec4(0.0f, 0.9f, 0.2f, 1.0f), "Model");

	std::vector<Mesh>& meshes = model->meshes; 

	if (ImGui::TreeNode("Meshes")) 
	{
		for (int i = 0; i < meshes.size(); ++i)
		{
			std::string name = "mesh_" + std::to_string(i);

			if (ImGui::TreeNode(std::move(name.c_str())))
			{

				if (ImGui::TreeNode("Render Data")) 
				{
					uint32_t positions = meshes[i].render_data.positions.size();
					uint32_t normals = meshes[i].render_data.normals.size();
					uint32_t uvs = meshes[i].render_data.uvs.size();
					uint32_t tangents = meshes[i].render_data.tangents.size();
					uint32_t bitangents = meshes[i].render_data.bitangents.size();
					uint32_t indices = meshes[i].render_data.indices.size();

					ImGui::Text(("positions: " + std::to_string(positions)).c_str());
					ImGui::Text(("normals: " + std::to_string(normals)).c_str());
					ImGui::Text(("uvs: " + std::to_string(uvs)).c_str());
					ImGui::Text(("tangents: " + std::to_string(tangents)).c_str());
					ImGui::Text(("bitangents: " + std::to_string(bitangents)).c_str());
					ImGui::Text(("indices: " + std::to_string(indices)).c_str());

					ImGui::TreePop(); 
				}

				if (ImGui::TreeNode("Material")) 
				{
					PBRMaterial& material = meshes[i].render_data.pbr_material;

					ImGui::Text("Diffuse	 Normal  MetallicRoughness");
					Create_Zoomable_Texture(material.diffuse_map->id);
					ImGui::SameLine(); 
					Create_Zoomable_Texture(material.normal_map->id);
					ImGui::SameLine(); 
					Create_Zoomable_Texture(material.metallicRoughness_map->id);

					ImGui::TreePop();
				}

				ImGui::TreePop(); 
			}
		}
		ImGui::TreePop(); 
	}

	ImGui::Separator();
}

void SceneHierarchyWidget::Create_Camera_Window(Camera* camera, float deltatime)
{
	ImGui::TextColored(ImVec4(0.0f, 0.9f, 0.2f, 1.0f), "Camera");
	ImGui::DragFloat("FOV", &camera->FOV, 10.0f * deltatime, 30.0f, 179.0f);
	ImGui::DragFloat("near", &camera->near, 1.0f * deltatime, -100.0f, 1.0f);
	ImGui::DragFloat("far", &camera->far, 1.0f * deltatime, 10.0f, 100.0f);
}

void SceneHierarchyWidget::Create_Zoomable_Texture(unsigned int id)
{
	float my_tex_w = (float)64;
	float my_tex_h = (float)64;
	{
		ImVec2 pos = ImGui::GetCursorScreenPos();
		ImVec2 uv_min = ImVec2(0.0f, 0.0f);                 // Top-left
		ImVec2 uv_max = ImVec2(1.0f, 1.0f);                 // Lower-right
		ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
		ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white
		ImGui::Image((ImTextureID)id, ImVec2(my_tex_w, my_tex_h), uv_min, uv_max, tint_col, border_col);
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			float region_sz = 32.0f;
			float region_x = ImGui::GetMousePos().x - pos.x - region_sz * 0.5f;
			float region_y = ImGui::GetMousePos().y - pos.y - region_sz * 0.5f;
			float zoom = 4.0f;
			if (region_x < 0.0f) { region_x = 0.0f; }
			else if (region_x > my_tex_w - region_sz) { region_x = my_tex_w - region_sz; }
			if (region_y < 0.0f) { region_y = 0.0f; }
			else if (region_y > my_tex_h - region_sz) { region_y = my_tex_h - region_sz; }
			ImGui::Text("Min: (%.2f, %.2f)", region_x, region_y);
			ImGui::Text("Max: (%.2f, %.2f)", region_x + region_sz, region_y + region_sz);
			ImVec2 uv0 = ImVec2((region_x) / my_tex_w, (region_y) / my_tex_h);
			ImVec2 uv1 = ImVec2((region_x + region_sz) / my_tex_w, (region_y + region_sz) / my_tex_h);
			ImGui::Image((ImTextureID)id, ImVec2(region_sz * zoom, region_sz * zoom), uv0, uv1, tint_col, border_col);
			ImGui::EndTooltip();
		}
	}
}
