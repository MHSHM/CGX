#pragma once

#include <Scene.h>
#include <ComponentManager.h>
#include <Transform.h>
#include <Model.h>
#include <PointLight.h>
#include <Framebuffer.h>
#include <SkyBox.h>

class SFinal : public Scene 
{
public:
	SFinal(class Game* _game);
	bool Init() override;
	void Load_Data() override;
	void Update(float deltatime) override;
	void Update_Components(float deltatime) override;
	void Draw() override;

public:
	ComponentManager<Transform> transform_manager;
	ComponentManager<Model> model_manager;
	ComponentManager<SkyBox> skybox_manager; 

	Framebuffer default_render_target;
};