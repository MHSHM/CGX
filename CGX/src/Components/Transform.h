#pragma once

#include <Component.h>

#include <vec3.hpp>
#include <mat4x4.hpp>

class Transform : public Component 
{
public:
	Transform(class Actor* _owner);
	void Update(float deltatime) override;
	void Clear() override; 

public:
	float scale; 
	glm::vec3 translation; 
	// in radians
	float angle_x;
	float angle_y;
	float angle_z;

	glm::vec3 axis;

	glm::mat4 translation_matrix;
	glm::mat4 scale_matrix;
	glm::mat4 rotation_x_matrix;
	glm::mat4 rotation_y_matrix;
	glm::mat4 rotation_z_matrix;
	glm::mat4 rotation_matrix; 
	glm::mat4 model_matrix;

	glm::vec3 local_x; 
	glm::vec3 local_y; 
	glm::vec3 local_z; 
};