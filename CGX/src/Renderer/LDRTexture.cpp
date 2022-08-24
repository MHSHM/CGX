#include "LDRTexture.h"
#include <glew.h>
#include <iostream>
#include <vector>

#include "../stb_image/stb_image.h"


bool LDRTexture::Generate_LDRTexture_From_File(const std::string& path, TextureParams _params, bool is_flip)
{
	params = _params;

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	if (is_flip) 
	{
		stbi_set_flip_vertically_on_load(true);
	}
	else 
	{
		stbi_set_flip_vertically_on_load(false);
	}

	int channels, format = GL_RGB;

	unsigned char* data = stbi_load(path.c_str(), (int*)&params.width, (int*)&params.height, &channels, 0);

	if (!data)
	{
		std::cerr << "Failed to load texture with path: " << path << '\n';
		delete data; 
		return false;
	}

	if (channels == 1) format = GL_RED;
	if (channels == 2) format = GL_RG;
	if (channels == 4) format = GL_RGBA;

	params.format = format; 
	params.internl_format  = format; 
	params.components_type = GL_UNSIGNED_BYTE;

	glTexImage2D(GL_TEXTURE_2D, 0, params.format, params.width, params.height, 0, params.format, params.components_type, data);

	if (params.generate_mips) 
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, params.mag_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, params.min_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, params.wrap_s);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, params.wrap_t);

	delete data;

	return true;

}

bool LDRTexture::Generate_HDRTexture_From_File(const std::string& path, TextureParams _params, bool is_flip)
{
	params = _params;

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	if (is_flip)
	{
		stbi_set_flip_vertically_on_load(true);
	}
	else
	{
		stbi_set_flip_vertically_on_load(false);
	}

	int channels, format = GL_RGB;

	float* data = stbi_loadf(path.c_str(), (int*)&params.width, (int*)&params.height, &channels, 0);

	if (!data)
	{
		std::cerr << "Failed to load texture with path: " << path << '\n';
		delete data;
		return false;
	}

	switch (channels)
	{
	case 1: 
	{
		params.internl_format = GL_R32F; 
		params.format = GL_RED; 
	}
	break;

	case 2: 
	{
		params.internl_format = GL_RG32F;
		params.format = GL_RG;
	}
	break;

	case 3: 
	{
		params.internl_format = GL_RGB32F;
		params.format = GL_RGB;
	}
	break; 

	case 4: 
	{
		params.internl_format = GL_RGBA32F;
		params.format = GL_RGBA;
	}
	break;
	}

	params.components_type = GL_FLOAT;

	glTexImage2D(GL_TEXTURE_2D, 0, params.internl_format, params.width, params.height, 0, params.format, params.components_type, data);

	if (params.generate_mips)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, params.mag_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, params.min_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, params.wrap_s);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, params.wrap_t);

	delete data;

	return true;
}

void LDRTexture::Generate(TextureParams _params, const std::vector<glm::vec3>& data)
{
	params = _params; 

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexImage2D(GL_TEXTURE_2D, 0, params.internl_format, params.width, params.height, 0, params.format, params.components_type, data.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, params.mag_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, params.min_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, params.wrap_s);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, params.wrap_t);
}

void LDRTexture::Bind(unsigned int index)
{
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, id);
}

void LDRTexture::UnBind()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
