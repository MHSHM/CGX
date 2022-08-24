#pragma once

#include <Component.h>
#include <vec3.hpp>

class PointLight : public Component 
{
public:
	PointLight(class Actor* _owner); 

	void Update(float deltatime) override; 
	void Clear() override; 

public:
	glm::vec3 light_position;
	glm::vec3 light_intensity; 
	float radius = 1.0f; 
	float radius_scalar = 100.0f; 
};