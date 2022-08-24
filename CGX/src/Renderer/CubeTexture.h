#pragma once

#include <Texture.h>

class CubeTexture : public Texture
{
public:
	CubeTexture() = default;

public:
	bool Generate(TextureParams _params, const std::vector<std::string>& faces_path);
	void Bind(unsigned int index); 
	void UnBind();
};