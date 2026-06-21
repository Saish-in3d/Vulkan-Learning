#define GLFW_INCLUDE_Vulkan

#include "glm/glm.hpp"
#include "GLFW/glfw3.h"
#include "vulkan/vulkan.h"

#include <iostream>

int main()
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	if(!glfwInit())
	{
		return -1;
	}
	GLFWwindow* window = glfwCreateWindow(800, 600, "Vulkan Window", nullptr, nullptr);
	if(!window)
	{
		glfwTerminate();
		return -1;
	}
	while(!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
	}

	return 0;
}