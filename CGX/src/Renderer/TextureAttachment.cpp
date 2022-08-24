#include "TextureAttachment.h"
#include <glew.h>
#include <iostream>

void TextureAttachment::Generate(TextureParams _params)
{
	params = _params;

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexImage2D(GL_TEXTURE_2D, 0, params.internl_format, params.width, params.height, 0, params.format, params.components_type, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, params.mag_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, params.min_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, params.wrap_s);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, params.wrap_t);
}

void TextureAttachment::Generate_Cube_Map(TextureParams _params)
{
	params = _params; 

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);

	for (int i = 0; i < 6; ++i) 
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, params.internl_format, params.width, params.height, 0, params.format, params.components_type, 0);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, params.mag_filter);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, params.min_filter);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, params.wrap_s);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, params.wrap_t);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, params.wrap_r);

	if (params.generate_mips) 
	{
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	}

}

void TextureAttachment::Bind(unsigned int index)
{
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, id);
}

void TextureAttachment::Bind_Cube_Map(unsigned int index)
{
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);
}

void TextureAttachment::Bind_As_Image(unsigned int index, unsigned int access, unsigned int format)
{
	glActiveTexture(GL_TEXTURE0 + index);
	glBindImageTexture(index, id, 0, 0, 0, access, format);
}

void TextureAttachment::UnBind()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}