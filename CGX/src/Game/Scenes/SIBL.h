#pragma once

#include <Scene.h>
#include <ComponentManager.h>
#include <Transform.h>
#include <Model.h>
#include <PointLight.h>
#include <Framebuffer.h>
#include <HDRSkyBox.h>
#include <Camera.h>

#include "../Editor/Editor.h"

class SIBL : public Scene 
{
public:
	SIBL(class Game* _game);
	bool Init() override;
	void Load_Data() override;
	void Update(float deltatime) override;
	void Update_Components(float deltatime) override;
	void Update_Shaders() override; 
	void Scene_Logic(float deltatime) override;
	void Draw() override;
	void Clear() override;


public:
	void Generate_GBuffer();
	void Draw_Scene_Refered_Image();
	void Draw_Display_Refered_Image();
	void Pre_SSAO_Data();
	void Calculate_SSAO();
	void Blur_SSAO();
	void HDREnv_To_CubeMap();
	void Draw_Cube_Map(); 
	void Generate_Irradiance_Map();
	void Generate_Pre_Filtered_Env_Map(); 
	void Generate_Integral_LUT(); 

public:

public:
	ComponentManager<Transform> transforms;
	ComponentManager<Model> models;
	ComponentManager<PointLight> pointlights;
	ComponentManager<HDRSkyBox> hdrskyboxes;
	ComponentManager<Camera> cameras;
	Editor editor;

public:
	// SSAO
	Framebuffer SSAO;
	Framebuffer blured_SSAO;
	std::vector<glm::vec3> samples;

	// Render Targets
	Framebuffer Gbuffer;
	Framebuffer cube_map; 
	Framebuffer scene_refered_render_target;
	Framebuffer display_refered_render_target;
	Framebuffer irradiance_map; 
	Framebuffer pre_filtered_env_map;
	Framebuffer integral_LUT; 

	Camera* scene_camera; 
};