#include "PointLight.h"
#include <Actor.h>
#include <Transform.h>

PointLight::PointLight(Actor* _owner):
	Component(_owner),
	light_position(glm::vec3(0.0f, 0.0f, 0.0f)),
	light_intensity(glm::vec3(5.0f, 5.0f, 5.0f))
{
	type = "PointLight";
}


void PointLight::Update(float deltatime)
{
	light_position = owner->Get_Component<Transform>()->translation;
}

void PointLight::Clear()
{
}
