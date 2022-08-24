#pragma once

class Renderer 
{
public:
	Renderer(class Game* _game);
	void Send_Data_To_GPU(const class Scene* scene);
	void Draw(); 

public:
	class Game* game; 
};