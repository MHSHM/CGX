#include <Game.h>
#include <SceneNode.h>
#include <thread>

#include <glfw3.h>

Scene::Scene(Game* _game):
	game(_game)
{

}

bool Scene::Init()
{
	return true;
}

void Scene::Initialize_Components()
{
}

void Scene::Load_Data()
{
}

void Scene::Update(float deltatime)
{
}

void Scene::Update_Components(float deltatime)
{
}

void Scene::Update_Shaders()
{
}

void Scene::Scene_Logic(float deltatime)
{
}

void Scene::Draw()
{
}

void Scene::Clear()
{
}
