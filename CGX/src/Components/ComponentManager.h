#pragma once

#include <Actor.h>

#include <vector>

#define MAX_COMPONENTS 100

template<class T>
class ComponentManager
{
public:

	ComponentManager() = default;

	inline void Init()
	{
		components.reserve(MAX_COMPONENTS);
	}

	inline T* Add_Component(class Actor* owner)
	{
		T cmp(owner);
		components.push_back(std::move(cmp));
		owner->components.push_back(&components.back());
		return &components.back();
	}

	inline void Remove_Component(Actor* owner, T* component)
	{
		if (!component) return;

		Actor* back_actor = components.back().owner;

		for (auto& cmp : back_actor->components) 
		{
			if (cmp == &components.back()) 
			{
				cmp = component; 
			}
		}

		auto owner_comp = std::find(owner->components.begin(), owner->components.end(), component); 

		if (owner_comp != owner->components.end()) 
		{
			std::iter_swap(owner_comp, owner->components.end() - 1); 
			owner->components.pop_back(); 
		}

		auto iter = std::find(components.begin(), components.end(), *component); 

		if (iter != components.end()) 
		{
			std::iter_swap(component, components.end() - 1);
			components.pop_back();
		}
	}

	std::vector<T> components;
};