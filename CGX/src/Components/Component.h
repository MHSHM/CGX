#pragma once

#include <string>

class Component
{
public:

	Component(class Actor* _owner);

	virtual void Initialize(); 
	virtual void Update(float deltatime) = 0;
	virtual void Clear() = 0;

public:
	class Actor* owner;
	std::string type;
};