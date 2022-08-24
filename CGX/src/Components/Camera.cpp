#include "Camera.h"
#include <Actor.h>
#include <Transform.h>
#include <gtc/matrix_transform.hpp>
#include <gtx/transform.hpp>
#include <SceneNode.h>
#include <Scene.h>
#include <Game.h>

Camera::Camera(Actor* _owner):
	Component(_owner)
{
}

void Camera::Initialize()
{
	aspect = (float)owner->scene_node->scene->game->window.width / owner->scene_node->scene->game->window.height;

	prespective_proj = glm::perspective(glm::radians(FOV), aspect, near, far);
}

void Camera::Update(float deltatime)
{
	Transform* transform = owner->Get_Component<Transform>();
	position = transform->translation; 

	glm::mat4 translation_mat = glm::translate(glm::mat4(1.0f), -position);
	view_rotation = glm::inverse(transform->rotation_y_matrix);

	forward = glm::vec3(transform->rotation_y_matrix * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
	right = glm::vec3(transform->rotation_y_matrix * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));

	view =  view_rotation * translation_mat;

	prespective_proj = glm::perspective(glm::radians(FOV), aspect, near, far);
}

void Camera::Clear()
{
}
