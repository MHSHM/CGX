#pragma once

#include <Scene.h>
#include <ComponentManager.h>
#include <Transform.h>
#include <Model.h>
#include <Framebuffer.h>

class STessellation : public Scene 
{
public:
	STessellation(class Game* _game); 
	bool Init() override; 
	void Load_Data() override;
	void Update(float deltatime) override; 
	void Update_Components(float deltatime) override; 
	void Draw() override; 
	void Clear() override; 

public:
	ComponentManager<Transform> transforms; 
	ComponentManager<Model> models; 

public:
	Framebuffer render_target; 

	unsigned int vertex_array_id; 
	unsigned int positions_buffer; 
	unsigned int uvs_buffer; 
};