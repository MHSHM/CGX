#include "SceneNodeManager.h"
#include "Scene.h"
#include "SceneNode.h"
#include "Actor.h"
#include "Component.h"

SceneNode* SceneNodeManager::Create_Scene_Node(Scene* scene, const std::string& _tag)
{
	scene->nodes.push_back(SceneNode(scene));
	scene->nodes.back().tag = _tag;
	scene->nodes.back().children.reserve(MAX_CHILDREN);
	scene->actors.emplace_back(Actor(&scene->nodes.back()));
	scene->nodes.back().actor = &(scene->actors.back());
	scene->nodes_map[_tag] = &scene->nodes.back();
	scene->nodes_ptrs.push_back(&scene->nodes.back());
	return &scene->nodes.back();
}

void SceneNodeManager::Remove_Scene_Node(Scene* scene, SceneNode* scene_node)
{
	for (auto& child : scene_node->children)
	{
		Remove_Scene_Node(scene, child);
	}

	// Here to delete a scene node we swap the node with the back of the vector
	// to avoid shifting when using erase
	// we still need to fix the children of the back as they will be pointing
	// to different parent after swapping
	SceneNode* back_scene_node = &scene->nodes.back();

	scene->nodes_map[back_scene_node->tag] = scene_node;

	if (back_scene_node->parent)
	{
		for (auto& child : back_scene_node->parent->children)
		{
			if (child == back_scene_node)
			{
				child = scene_node;
			}
		}
	}

	if (scene_node->parent)
	{
		scene_node->parent->Remove_Child(scene_node);
	}

	for (auto& actor : scene->actors)
	{
		if (actor.scene_node == back_scene_node)
		{
			actor.scene_node = scene_node;
		}
	}

	for (auto& child : back_scene_node->children)
	{
		child->parent = scene_node;
	}

	Remove_Actor(scene, scene_node->actor);

	std::iter_swap(scene_node, scene->nodes.end() - 1);

	scene->nodes.pop_back();
}

void SceneNodeManager::Remove_Actor(Scene* scene, Actor* actor)
{
	scene->nodes_map[scene->actors.back().scene_node->tag]->actor = actor;

	auto iter = std::find(scene->actors.begin(), scene->actors.end(), *actor);

	if (iter != scene->actors.end())
	{
		iter->Clear();

		for (auto& scene_node : scene->nodes)
		{
			if (scene_node.actor == &scene->actors.back())
			{
				scene_node.actor = actor;
			}
		}

		for (auto& cmp : scene->actors.back().components)
		{
			cmp->owner = actor;
		}

		std::iter_swap(actor, scene->actors.end() - 1);
		scene->actors.pop_back();
	}
}
