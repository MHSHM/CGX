#pragma once

#include <vector>
#include <string>
#include <vec2.hpp>
#include <vec3.hpp>
#include <VertexArrayObject.h>

struct PhongMaterial
{
	class LDRTexture* diffuse_map;
	class LDRTexture* specular_map;
	class LDRTexture* normal_map;
	class LDRTexture* height_map;

	glm::vec3 ka = glm::vec3(0.23125f, 0.23125f, 0.23125f);
	glm::vec3 kd = glm::vec3(0.2775f, 0.2775f, 0.2775f);
	glm::vec3 ks = glm::vec3(0.773911f, 0.773911f, 0.773911f);

	float shininess = 89.6f;
};

struct PBRMaterial 
{
	class LDRTexture* diffuse_map; 
	class LDRTexture* normal_map; 
	class LDRTexture* metallicRoughness_map; 
};

struct RenderData
{
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> tangents;
	std::vector<glm::vec3> bitangents;
	std::vector<unsigned int> indices;

	PhongMaterial material;
	PBRMaterial pbr_material;
};

class Mesh 
{
public:
	Mesh();
	void Init(class Model* model); 

public:
	RenderData render_data;
	VertexArrayObject vao;
	unsigned int material_index = 0;
};