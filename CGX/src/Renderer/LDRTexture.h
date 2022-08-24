#pragma once

#include <Texture.h>
#include <string>
#include <vec3.hpp>

class LDRTexture : public Texture 
{
public:
	LDRTexture() = default;

public:
	bool Generate_LDRTexture_From_File(const std::string& path, TextureParams _params, bool is_flip = true);
	bool Generate_HDRTexture_From_File(const std::string& path, TextureParams _params, bool is_flip = true);
	void Generate(TextureParams _params, const std::vector<glm::vec3>& data);
	void Bind(unsigned int index); 
	void UnBind();
};