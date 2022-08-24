#include "SkyBox.h"
#include <Actor.h>
#include <glew.h>

SkyBox::SkyBox(Actor* _owner):
	Component(_owner)
{
	type = "SkyBox";
}

void SkyBox::Initialize()
{
	TextureParams params;

	params.width = 512;
	params.height = 512;
	params.internl_format = GL_RGB;
	params.format = GL_RGB;
	params.components_type = GL_UNSIGNED_BYTE;
	params.mag_filter = GL_LINEAR;
	params.min_filter = GL_LINEAR_MIPMAP_LINEAR;
	params.wrap_s = GL_CLAMP_TO_EDGE;
	params.wrap_t = GL_CLAMP_TO_EDGE;
	params.generate_mips = true;
	
	env_map.Generate(params, faces); 
}


void SkyBox::Update(float deltatime)
{
}

void SkyBox::Clear()
{
	env_map.Clear();
}
