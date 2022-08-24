#pragma once

#include <SceneNode.h>
#include <Actor.h>
#include <LDRTexture.h>
#include <vector>
#include <string>
#include <unordered_map>

#define MAX_NODES 100
#define MAX_ACTORS 100
#define MAX_TEXTURES 100

class Scene
{
public:
	Scene(class Game* _game);

public:
	virtual bool Init();
	virtual void Initialize_Components(); 
	virtual void Load_Data();
	virtual void Update(float deltatime);
	virtual void Update_Components(float deltatime);
	virtual void Update_Shaders(); 
	virtual void Scene_Logic(float deltatime); 
	virtual void Draw();
	virtual void Clear();

public:
	std::vector<SceneNode> nodes;
	std::vector<SceneNode*> nodes_ptrs;
	std::vector<Actor> actors;
	std::unordered_map<std::string, class SceneNode*> nodes_map;
	std::vector<class SceneNode*> dead_scene_nodes;
	std::vector<LDRTexture> textures;
	std::unordered_map<std::string, class LDRTexture*> textures_map;

	class Game* game;	 
};