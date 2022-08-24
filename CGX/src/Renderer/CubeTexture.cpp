#include "CubeTexture.h"
#include <glew.h>
#include <iostream>

#include "../stb_image/stb_image.h"


bool CubeTexture::Generate(TextureParams _params, const std::vector<std::string>& faces_path)
{
	params = _params; 

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);

	//stbi_set_flip_vertically_on_load(true);

	int channels, format = GL_RGB;

	for (int i = 0; i < faces_path.size(); ++i)
	{
		unsigned char* data = stbi_load(faces_path[i].c_str(), (int*)&params.width, (int*)&params.height, &channels, 0);

		if (!data)
		{
			std::cerr << "Failed to load texture with path: " << faces_path[i] << '\n';
			return false;
		}

		if (channels == 1) format = GL_RED;
		if (channels == 2) format = GL_RG;
		if (channels == 4) format = GL_RGBA;

		params.format = format; 
		params.internl_format = format;
		params.components_type = GL_UNSIGNED_BYTE; 

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, params.format, params.width, params.height, 0, params.format, params.components_type, data);
	}

	if (params.generate_mips)
	{
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, params.mag_filter);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, params.min_filter);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, params.wrap_s);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, params.wrap_t);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, params.wrap_r);


	return true;
}

void CubeTexture::Bind(unsigned int index)
{
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);
}

void CubeTexture::UnBind()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
