#include "Framebuffer.h"
#include <glew.h>
#include <iostream>

Framebuffer::Framebuffer():
	framebuffer_id(0)
{
	color_buffers.reserve(5);
	depth_buffers.reserve(5);
}

void Framebuffer::Generate_Framebuffer()
{
	glGenFramebuffers(1, &framebuffer_id);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);
}

void Framebuffer::Generate_Texture_Attachment(const Attachment& attach_type, const TextureParams& params)
{
	TextureAttachment attachment;

	switch (attach_type)
	{
	case Attachment::Color: 
	{
		attachment.Generate(params);

		color_buffers.push_back(std::move(attachment));
	}
	break;
	case Attachment::Depth: 
	{
		attachment.Generate(params);
		depth_buffers.push_back(std::move(attachment));
	}
	break;
	case Attachment::ColorCube: 
	{
		attachment.Generate_Cube_Map(params);

		color_buffers.push_back(std::move(attachment));
	}
	break;
	case Attachment::DepthCube: 
	{
		attachment.Generate_Cube_Map(params);

		depth_buffers.push_back(std::move(attachment)); 
	}
	break;
	default:
		break;
	}
}


bool Framebuffer::Configure(int depth_index, int color_index)
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);

	if(depth_index > -1)
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_buffers[depth_index].id, 0);

	std::vector<unsigned int> attachments; 

	for (int i = 0; i < color_index + 1; ++i)
	{
		attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, color_buffers[i].id, 0);
	}

	glDrawBuffers(attachments.size(), attachments.data());

	if (color_buffers.size() == 0) 
	{
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return false;
	}

	return true;
}

void Framebuffer::Bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id); 
}

void Framebuffer::Unbind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0); 
}

void Framebuffer::Clear()
{

	for (auto& attach : color_buffers) 
	{
		attach.Clear(); 
	}

	for (auto& attach : depth_buffers) 
	{
		attach.Clear(); 
	}

	glDeleteFramebuffers(1, &framebuffer_id); 
}
