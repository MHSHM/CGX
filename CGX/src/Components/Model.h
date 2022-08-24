#pragma once

#include <Component.h>
#include <Mesh.h>

class Model : public Component
{
public:
	Model(class Actor* _owner);

	void Update(float deltatime) override;
	void Clear() override;

public:

	std::vector<Mesh> meshes;
};