#include "Window.h"
#include <glfw3.h>
#include <Game.h>
#include <iostream>

Window::Window(uint32_t _width, uint32_t _height):
    width(_width),
    height(_height),
	window_ptr(nullptr)
{

}

bool Window::Initialize()
{
	window_ptr = glfwCreateWindow(width, height, "CGX", NULL, NULL);
	if (!window_ptr)
	{
		LOG_ERR("failed to create window!\n");
		return false;
	}

	glfwMakeContextCurrent(window_ptr);
	glfwSetWindowUserPointer(window_ptr, (void*)this);

	float aspect = (float)width / (float)height;
	prespective_proj = glm::perspective(glm::radians(FOV), aspect, 0.1f, 100.0f);

	return true; 

}
