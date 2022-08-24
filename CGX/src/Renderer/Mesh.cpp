#include "Mesh.h"
#include "../Components/Model.h"
#include <Actor.h>
#include <SceneNode.h>
#include <Scene.h>
#include <Game.h>
#include <glew.h>

Mesh::Mesh()
{
}

void Mesh::Init(Model* model)
{
	Scene* scene = model->owner->scene_node->scene;
	Game* game = model->owner->scene_node->scene->game;

	TextureParams params;
	params.mag_filter = GL_LINEAR;
	params.min_filter = GL_LINEAR;
	params.wrap_s = GL_REPEAT;
	params.wrap_t = GL_REPEAT;
	params.generate_mips = false;
	render_data.pbr_material.diffuse_map = game->loader.Load_Texture(scene, "Assets/black.png", params);
	render_data.pbr_material.normal_map = game->loader.Load_Texture(scene, "Assets/black.png", params);
	render_data.pbr_material.metallicRoughness_map = game->loader.Load_Texture(scene, "Assets/black.png", params);
}
