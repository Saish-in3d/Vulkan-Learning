#pragma once
#include "common.h"

namespace VKFW {
	class VulkanApp 
	{
	public:
		void run();


	private:
		void initWindow();
		void initVulkan();
		void mainLoop();
		void cleanup();

		GLFWwindow* window;

		int width = 1280;
		int height = 720;

	};
}