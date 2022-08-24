#pragma once

#include <Scene.h>
#include <ComponentManager.h>
#include <Transform.h>
#include <Model.h>
#include <PointLight.h>
#include <SkyBox.h>
#include <Framebuffer.h>
#include "../Editor/Editor.h"

class SEnvironment : public Scene
{
public:
	SEnvironment(class Game* _game);
	bool Init() override;
	void Load_Data() override;
	void Update(float deltatime) override;
	void Update_Components(float deltatime) override;
	void Draw() override;

public:
	ComponentManager<Transform> transform_manager;
	ComponentManager<Model> model_manager;
	ComponentManager<PointLight> pointlight_manager;
	ComponentManager<SkyBox> cubemap_manager;

	Editor editor; 

	Framebuffer environment_world_render_target;

};