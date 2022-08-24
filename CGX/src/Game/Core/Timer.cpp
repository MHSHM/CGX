#include "Timer.h"

#include <gtx/transform.hpp>
#include <glfw3.h>

float Timer::Get_Deltatime()
{
	float delta_time = glfwGetTime() - time_since_last_frame;
	time_since_last_frame = glfwGetTime();
	delta_time = round(delta_time * 100.0f) / 100.0f;
	return delta_time;
}
