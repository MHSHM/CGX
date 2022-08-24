#pragma once

#include <Scene.h>
#include <ComponentManager.h>
#include <Model.h>
#include <Transform.h>
#include <PointLight.h>
#include <Framebuffer.h>

class SNormalMapping : public Scene
{
public:
	SNormalMapping(class Game* _game);
	bool Init() override;
	void Load_Data() override; 
	void Update(float deltatime) override;
	void Update_Components(float deltatime) override;
	void Draw() override;
	void Clear() override; 

public:
	void Generate_Depth_Maps();
public:
	ComponentManager<Transform> transforms; 
	ComponentManager<Model> models; 
	ComponentManager<PointLight> pointlights; 

public:
	bool draw_wireframes = false; 

public:
	// generate normal mapped plane
	Framebuffer render_target;
	// For shadow maps
	Framebuffer depth_render_target;
};