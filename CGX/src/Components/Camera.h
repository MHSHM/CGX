#pragma once

#include <Component.h>
#include <vec3.hpp>
#include <mat4x4.hpp>


class Camera : public Component 
{
public:
	Camera(class Actor* _owner);
	void Initialize() override; 
	void Update(float deltatime) override;
	void Clear() override; 

public:
	glm::vec3 position;
	glm::mat4 view; 
	glm::mat4 view_rotation;

	glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f); 
	glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f); 

	float aspect;
	float FOV = 90.0f;
	float near = 0.1f;
	float far = 100.0f;
	
	glm::mat4 prespective_proj;
};