#pragma once

#include <Scene.h>
#include <ComponentManager.h>
#include <Transform.h>
#include <Model.h>
#include <Camera.h>
#include <Framebuffer.h>

#include "../Editor/Editor.h"

static bool generate = false;

class SRayTracing : public Scene
{
public:
	SRayTracing(class Game* _game); 
	bool Init() override;
	void Initialize_Components() override; 
	void Load_Data() override;
	void Update(float deltatime) override;
	void Update_Components(float deltatime) override;
	void Update_Shaders() override;
	void Scene_Logic(float deltatime) override;
	void Draw() override;
	void Clear() override;

public:
	// Shader buffer storages
	unsigned int indices_buffer; 
	unsigned int positions_buffer; 
	unsigned int normals_buffer; 

public:
	void Initialize_Buffers(); 
	void Generate_Image(); 
	void Draw_Image_On_Screen(); 

public:
	ComponentManager<Transform> transforms;
	ComponentManager<Model> models;
	ComponentManager<Camera> cameras;
	Editor editor;

public:
	TextureAttachment image2D;
	Framebuffer render_target;
};