#pragma once

#include <vector>
#include <TextureAttachment.h>

enum class Attachment
{
	Color,
	Depth,
	ColorCube, 
	DepthCube
};

class Framebuffer 
{
public:
	Framebuffer(); 
	void Generate_Framebuffer();
	void Generate_Texture_Attachment(const Attachment& attach_type, const TextureParams& params);
	bool Configure(int depth_index, int color_index);

	void Bind() const;
	void Unbind() const;
	void Clear(); 

public:

	unsigned int framebuffer_id;
	std::vector<TextureAttachment> color_buffers;
	std::vector<TextureAttachment> depth_buffers;
};