#pragma once

#include <Texture.h>

class TextureAttachment : public Texture
{
public:
	TextureAttachment() = default; 

public:
	void Generate(TextureParams _params);
	void Generate_Cube_Map(TextureParams _params);
	void Bind(unsigned int index); 
	void Bind_Cube_Map(unsigned int index); 
	void Bind_As_Image(unsigned int index, unsigned int access, unsigned int format);
	void UnBind(); 
};