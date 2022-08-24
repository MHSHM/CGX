#pragma once

#include <cstdint>
#include <mat4x4.hpp>

class Window 
{
public:
	explicit Window(uint32_t _width, uint32_t _height); 

public:
	bool Initialize(); 
public:
	uint32_t width; 
	uint32_t height; 

	class GLFWwindow* window_ptr;

	float FOV = 90.0f;
	glm::mat4x4 prespective_proj;
};