#pragma once

#include <string>
#include <vector>


enum class TextureType 
{
	LDR = 0, 
	HDR = 1
};

struct TextureParams 
{
	unsigned int width = 0;
	unsigned int height = 0;
	unsigned int internl_format = 0;
	unsigned int format = 0;
	unsigned int components_type = 0;
	unsigned int mag_filter = 0; 
	unsigned int min_filter = 0; 
	unsigned int wrap_s = 0; 
	unsigned int wrap_t = 0; 
	unsigned int wrap_r = 0; 

	TextureType type = TextureType::LDR;

	bool generate_mips = true;
};

class Texture 
{
public:
	Texture();
	void Clear(); 

public:
	unsigned int id; 
	TextureParams params; 
};