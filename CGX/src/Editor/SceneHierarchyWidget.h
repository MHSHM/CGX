#pragma once

#include "Widget.h"

#include <vector>
#include <unordered_map>

class SceneHierarchyWidget : public Widget 
{
public:
	SceneHierarchyWidget(std::vector<class SceneNode*>& nodes);

public:
	void Initialize() override;
	void Update(float deltatime) override;
	void ShutDown() override;

public:
	void Create_Parent_Child_Hierarchy(class SceneNode* node, float deltatime);
	void Display_Components_Window(class SceneNode* node, float deltatime);
	void Create_Transform_Window(class Transform* transform, float deltatime);
	void Create_Skybox_Window(class SkyBox* skybox);
	void Create_Quadlight_Window(class QuadLight* quadlight, float deltatime);
	void Create_Pointlight_Window(class PointLight* pointlight);
	void Create_Model_Window(class Model* model);
	void Create_Camera_Window(class Camera* camera, float deltatime);
	void Create_Zoomable_Texture(unsigned int id); 

public:
	std::vector<class SceneNode*> m_scene_nodes;
};