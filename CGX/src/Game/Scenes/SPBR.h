#pragma once

#include <Scene.h>
#include <Framebuffer.h>
#include <ComponentManager.h>
#include <Model.h>
#include <Transform.h>
#include <PointLight.h>
#include <QuadLight.h>
#include "../Editor/Editor.h"

void Random_Points_In_Hemisphere(std::vector<glm::vec3>& samples, uint32_t N);

class SPBR : public Scene 
{
public:
	SPBR(class Game* _game); 
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
	void Calculate_Scene_Exposure();
	void Draw_Display_Refered_Image();
	void Pre_SSAO_Data(); 
	void Calculate_SSAO();
	void Blur_SSAO();

public:
	ComponentManager<Transform> transforms;
	ComponentManager<Model> models;
	ComponentManager<PointLight> pointlights;
	ComponentManager<QuadLight> quadlights;

public:
	// SSAO
	Framebuffer SSAO;
	Framebuffer blured_SSAO;
	std::vector<glm::vec3> samples;
	
	// LTC
	LDRTexture* ltc_1; 
	LDRTexture* ltc_2; 

	// Render Targets
	Framebuffer Gbuffer;
	Framebuffer scene_refered_render_target;
	Framebuffer display_refered_render_target;

	// Editor
	Editor editor;
};