#pragma once

#include <Scene.h>
#include <ComponentManager.h>
#include <Transform.h>
#include <Model.h>
#include <PointLight.h>
#include <SkyBox.h>
#include <Framebuffer.h>
#include "../Editor/Editor.h"

class SShadow : public Scene
{
public:
	SShadow(class Game* _game);
	bool Init() override;
	void Load_Data() override; 
	void Update(float deltatime) override;
	void Update_Components(float deltatime) override;
	void Draw() override;
	void Clear() override;

public:
	void Generate_Depth_Maps();
	void Draw_Scene_With_Shadows(); 
public:
	ComponentManager<Transform> transform_manager;
	ComponentManager<Model> model_manager;
	ComponentManager<PointLight> pointlight_manager;
	ComponentManager<SkyBox> cubemap_manager;

	Editor editor; 

	Framebuffer shadow_render_target;
	Framebuffer depth_maps_buffer;

	// light camera
	float near = 0.1f; 
	float far = 25.0f; 
	float FOV = 90.0f; 
};