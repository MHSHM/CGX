#include "Transform.h"
#include <gtc/matrix_transform.hpp>
#include <gtx/transform.hpp>
#include <algorithm>

Transform::Transform(Actor* _owner):
	Component(_owner),
	scale(1.0f), 
	angle_x(0.0f),
	angle_y(0.0f),
	angle_z(0.0f),
	axis(glm::vec3(0.0f, 1.0f, 0.0f)),
	translation(glm::vec3(0.0f, 0.0f, -2.0f)),
	model_matrix(glm::mat4(1.0f)),
	local_x(glm::vec3(1.f, 0.0f, 0.0f)),
	local_y(glm::vec3(0.0f, 1.0f, 0.0f)), 
	local_z(glm::vec3(0.0f, 0.0f, 1.0f))
{
	type = "Transform";
}


void Transform::Update(float deltatime)
{
	scale_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(scale));
	translation_matrix = glm::translate(glm::mat4(1.0f), translation);

	rotation_x_matrix = glm::rotate(angle_x, local_x);
	rotation_y_matrix = glm::rotate(angle_y, local_y);
	rotation_z_matrix = glm::rotate(angle_z, local_z);

	rotation_matrix = rotation_z_matrix * rotation_y_matrix * rotation_x_matrix;

	model_matrix = translation_matrix * rotation_matrix * scale_matrix;
}

void Transform::Clear()
{
}
