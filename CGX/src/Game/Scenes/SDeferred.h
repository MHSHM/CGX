#pragma once

#include <Scene.h>
#include <Framebuffer.h>
#include <ComponentManager.h>
#include <Transform.h>
#include <Model.h>
#include <PointLight.h>

class SDeferred : public Scene 
{
public:
	SDeferred(class Game* _game); 
	bool Init() override; 
	void Load_Data() override; 
	void Update(float deltatime) override;
	void Update_Components(float deltatime) override; 
	void Draw() override; 
	void Clear() override; 

public:
	void Draw_First_Pass(); 
	void Draw_Second_Pass(); 

public:
	ComponentManager<Transform> transforms;
	ComponentManager<PointLight> pointlights;
	ComponentManager<Model> models;

public:
	Framebuffer GBuffer; 
	Framebuffer render_target; 
};