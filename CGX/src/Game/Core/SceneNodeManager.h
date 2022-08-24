#pragma once

#include <string>

class SceneNodeManager 
{

public:
	SceneNodeManager() = default;

public:
	class SceneNode* Create_Scene_Node(class Scene* scene, const std::string& _tag);
	void Remove_Scene_Node(class Scene* scene, SceneNode* scene_node);
	void Remove_Actor(class Scene* scene, class Actor* actor);
};