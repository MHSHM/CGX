#pragma once

#include <Scene.h>
#include "../Editor/Editor.h"
#include <ComponentManager.h>
#include <Transform.h>
#include <Model.h>
#include <Camera.h>
#include <Framebuffer.h>

class SVXGI : public Scene 
{
public:
	SVXGI(class Game* _game);

	bool Init() override;
	void Load_Data() override;
	void Update(float deltatime) override;
	void Update_Components(float deltatime) override;
	void Update_Shaders() override;
	void Scene_Logic(float deltatime) override;
	void Draw() override;
	void Clear() override;

public:
	void Voxelize(); 
	void Visualize_Voxels(); 
	void Cone_Tracing(); 

public:
	ComponentManager<Transform> transforms; 
	ComponentManager<Model> models; 
	ComponentManager<Camera> cameras; 
	Editor editor;

public:
	Framebuffer render_target_1;
	Framebuffer render_target_2;

public:
	unsigned int voxel_grid;
	unsigned int width = 128; 
	unsigned int height = 128;
	unsigned int depth = 128;
};