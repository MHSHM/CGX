#include "Loader.h"

#include <Game.h>
#include <Model.h>
#include <iostream>
#include <vec3.hpp>
#include <vec2.hpp>
#include <glew.h>
#include <LDRTexture.h>
#include <future>
#include <chrono>

#include "../Filesystem/Filesystem.h"

Loader::Loader(Game* _game):
	game(_game)
{
}


bool Loader::Load(SceneNode* node, const std::string& path)
{
	Assimp::Importer importer;

	imported_model = importer.ReadFile(path,
		aiProcess_Triangulate |
		aiProcess_CalcTangentSpace |
		aiProcess_JoinIdenticalVertices);

	if (!imported_model) {
		std::cerr << "failed to load scene/model with path: " << path << '\n';
		return false;
	}

	Load_Model(node->actor->Get_Component<Model>(), path);
	
	return true;
}

void Loader::Load_Model(Model* model, const std::string& model_path)
{
	model->meshes.reserve(imported_model->mNumMeshes);
	
	std::vector<std::future<void>> futures; 
	futures.reserve(imported_model->mNumMeshes);
	
	for (int i = 0; i < imported_model->mNumMeshes; ++i)
	{
		aiMesh* curr_mesh = imported_model->mMeshes[i];

		Task t([this, model, curr_mesh]() { Load_Mesh(model, curr_mesh); });
		futures.push_back(t.GetFuture());
		game->pool.Schedule(std::move(t));
	}
	
	for (auto& future : futures) 
	{
		future.wait(); 
	}

	for (auto& mesh : model->meshes) 
	{
		Load_Material(&mesh, model, model_path);
	}
}

void Loader::Load_Mesh(Model* model, aiMesh* curr_mesh)
{
	Mesh mesh;

	mesh.Init(model);

	mesh.material_index = curr_mesh->mMaterialIndex;
	mesh.render_data.positions.reserve(curr_mesh->mNumVertices);
	mesh.render_data.normals.reserve(curr_mesh->mNumVertices);
	mesh.render_data.uvs.reserve(curr_mesh->mNumVertices);
	mesh.render_data.tangents.reserve(curr_mesh->mNumVertices);
	mesh.render_data.bitangents.reserve(curr_mesh->mNumVertices);
	mesh.render_data.indices.reserve(curr_mesh->mNumFaces * 3);

	for (int j = 0; j < curr_mesh->mNumVertices; ++j)
	{
		mesh.render_data.positions.emplace_back(glm::vec3(curr_mesh->mVertices[j].x, curr_mesh->mVertices[j].y, curr_mesh->mVertices[j].z));
	}

	if (curr_mesh->HasNormals())
	{
		for (int j = 0; j < curr_mesh->mNumVertices; ++j)
		{
			mesh.render_data.normals.emplace_back(glm::vec3(curr_mesh->mNormals[j].x, curr_mesh->mNormals[j].y, curr_mesh->mNormals[j].z));
		}
	}

	if (curr_mesh->HasTextureCoords(0))
	{
		for (int j = 0; j < curr_mesh->mNumVertices; ++j)
		{
			mesh.render_data.uvs.emplace_back(glm::vec2(curr_mesh->mTextureCoords[0][j].x, curr_mesh->mTextureCoords[0][j].y));
		}
	}

	if (curr_mesh->HasTangentsAndBitangents())
	{
		for (int j = 0; j < curr_mesh->mNumVertices; ++j)
		{
			glm::vec3 tangent = glm::vec3(curr_mesh->mTangents[j].x, curr_mesh->mTangents[j].y, curr_mesh->mTangents[j].z);
			glm::vec3 bitangent = glm::vec3(curr_mesh->mBitangents[j].x, curr_mesh->mBitangents[j].y, curr_mesh->mBitangents[j].z);
			mesh.render_data.tangents.emplace_back(std::move(tangent));
			mesh.render_data.bitangents.emplace_back(std::move(bitangent));
		}
	}

	for (int j = 0; j < curr_mesh->mNumFaces; ++j)
	{
		mesh.render_data.indices.emplace_back(curr_mesh->mFaces[j].mIndices[0]);
		mesh.render_data.indices.emplace_back(curr_mesh->mFaces[j].mIndices[1]);
		mesh.render_data.indices.emplace_back(curr_mesh->mFaces[j].mIndices[2]);
	}

	std::lock_guard<std::mutex> lock(mutex);
	model->meshes.push_back(std::move(mesh));
}

void Loader::Load_Material(Mesh* mesh, Model* model, const std::string& model_path)
{
	aiMaterial* material = imported_model->mMaterials[mesh->material_index];

	std::string directory; 
	aiString path;

	Get_Directory_From_Path(directory, model_path);

	TextureParams params; 
	params.mag_filter = GL_LINEAR; 
	params.min_filter = GL_LINEAR_MIPMAP_LINEAR; 
	params.wrap_s = GL_REPEAT;
	params.wrap_t = GL_REPEAT;
	params.generate_mips = true;

	if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
	{
		material->GetTexture(aiTextureType_DIFFUSE, 0, &path);
		aiString final_path(std::move(directory));
		final_path.Append(path.C_Str());
		mesh->render_data.pbr_material.diffuse_map = Load_Texture(model->owner->scene_node->scene, final_path.C_Str(), params);
	}

	if (material->GetTextureCount(aiTextureType_UNKNOWN) > 0)
	{
		material->GetTexture(aiTextureType_UNKNOWN, 0, &path);
		aiString final_path(std::move(directory));
		final_path.Append(path.C_Str());
		mesh->render_data.pbr_material.metallicRoughness_map = Load_Texture(model->owner->scene_node->scene, final_path.C_Str(), params);
	}


	if (material->GetTextureCount(aiTextureType_NORMALS) > 0)
	{
		material->GetTexture(aiTextureType_NORMALS, 0, &path);
		aiString final_path(std::move(directory));
		final_path.Append(path.C_Str());
		mesh->render_data.pbr_material.normal_map = Load_Texture(model->owner->scene_node->scene, final_path.C_Str(), params);
	}
}

LDRTexture* Loader::Load_Texture(Scene* scene, const std::string& path, TextureParams params, bool is_flip)
{
	if (scene->textures_map[path])
	{
		return scene->textures_map[path];
	}

	scene->textures.push_back(LDRTexture());

	switch (params.type)
	{
	case TextureType::LDR: 
	{
		if (!scene->textures.back().Generate_LDRTexture_From_File(path, params, is_flip))
		{
			return nullptr;
		}
	}
	break;

	case TextureType::HDR: 
	{
		if (!scene->textures.back().Generate_HDRTexture_From_File(path, params, is_flip))
		{
			return nullptr;
		}
	}
	break;
	}

	scene->textures_map[path] = &scene->textures.back();

	return &scene->textures.back();
}


Loader::~Loader()
{
}
