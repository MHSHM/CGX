#pragma once

#include <Component.h>

class HDRSkyBox : public Component 
{
public:
	HDRSkyBox(class Actor* _owner); 

	void Initialize(const std::string& env_path); 
	void Update(float deltatime) override; 
	void Clear() override; 

public:
	class LDRTexture* hdr_env_map = nullptr;
};