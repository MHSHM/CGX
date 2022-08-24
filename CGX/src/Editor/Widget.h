#pragma once

class Widget 
{
public:
	virtual void Initialize() = 0;
	virtual void Update(float deltatime) = 0;
	virtual void ShutDown() = 0;
};