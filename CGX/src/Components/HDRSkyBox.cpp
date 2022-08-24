#include "HDRSkyBox.h"

#include <Game.h>
#include <Actor.h>
#include <LDRTexture.h>
#include <glew.h>

HDRSkyBox::HDRSkyBox(Actor* _owner):
	Component(_owner)
{
}

void HDRSkyBox::Initialize(const std::string& env_path)
{
    TextureParams params;

    params.mag_filter = GL_LINEAR;
    params.min_filter = GL_LINEAR;
    params.wrap_s = GL_REPEAT;
    params.wrap_t = GL_REPEAT;
    params.type = TextureType::HDR;
    params.generate_mips = false;

    hdr_env_map = owner->scene_node->scene->game->loader.Load_Texture(owner->scene_node->scene, env_path, params);
}

void HDRSkyBox::Update(float deltatime)
{
}

void HDRSkyBox::Clear()
{
}
