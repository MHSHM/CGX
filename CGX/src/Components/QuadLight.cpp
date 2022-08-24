#include "QuadLight.h"
#include "Actor.h"
#include "Transform.h"
#include "Model.h"

#include <iostream>
#include <gtc/matrix_transform.hpp>
#include <gtx/transform.hpp>


QuadLight::QuadLight(Actor* _owner):
	Component(_owner)
{
	type = "QuadLight";
}

void QuadLight::Update(float deltatime)
{
	Transform* trasnform = owner->Get_Component<Transform>(); 
	Model* model		 = owner->Get_Component<Model>(); 
	Mesh& mesh           = model->meshes.back(); 

	vertices[0] = trasnform->model_matrix * glm::vec4(mesh.render_data.positions[0], 1.0f);
	vertices[1] = trasnform->model_matrix * glm::vec4(mesh.render_data.positions[1], 1.0f);
	vertices[2] = trasnform->model_matrix * glm::vec4(mesh.render_data.positions[2], 1.0f);
	vertices[3] = trasnform->model_matrix * glm::vec4(mesh.render_data.positions[3], 1.0f);
}

void QuadLight::Clear()
{

}
