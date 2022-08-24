#pragma once

#include <Component.h>
#include <CubeTexture.h>
#include <vector>

class SkyBox : public Component 
{
public:
	SkyBox(class Actor* owner);
	void Initialize();
	void Update(float deltatime) override; 
	void Clear() override;

public:
	CubeTexture env_map;
	std::vector<std::string> faces; 
};