#include <Texture.h>
#include <glew.h>
#include <iostream>
#include <Game.h>

#define STB_IMAGE_IMPLEMENTATION

#include "../stb_image/stb_image.h"

Texture::Texture()
{
}

void Texture::Clear()
{
	glDeleteTextures(1, &id);
}
