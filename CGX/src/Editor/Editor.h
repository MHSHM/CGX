#pragma once

#define MAX_THREADS 12
#define MAX_WIDGETS 12

#include <vector>

class Editor 
{
public:
	bool Initialize(class Game* _game);
	void Setup(class Scene* scene);
	void Update(float deltatime);
	void Draw(); 
	void ShutDown();

private:
	class Game* game = nullptr;
	std::vector<class Widget*> m_widgets;
};