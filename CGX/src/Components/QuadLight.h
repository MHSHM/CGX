#pragma once

#include "Component.h"

#include <vec3.hpp>
#include <vec4.hpp>


class QuadLight : public Component
{
public:
	QuadLight(class Actor* _owner);

	void Update(float deltatime) override;
	void Clear() override;

public:
	glm::vec4 vertices[4];
	glm::vec3 intensity = glm::vec3(100.0f, 100.0f, 100.0f);
};