#pragma once

#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <Texture.h>

#include <mutex>

class Loader
{
public:
	Loader(class Game* _game);
	~Loader();

	bool Load(class SceneNode* node, const std::string& path);
	class LDRTexture* Load_Texture(class Scene* scene, const std::string& path, TextureParams params, bool is_flip = true);
	void Load_Model(class Model* model, const std::string& model_path);
	void Load_Mesh(class Model* model, class aiMesh* curr_mesh);
	void Load_Material(class Mesh* mesh, class Model* model, const std::string& model_path);

public:
	const aiScene* imported_model;
	class Game* game;
	std::mutex mutex; 
};
