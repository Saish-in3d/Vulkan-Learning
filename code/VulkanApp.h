#pragma once
#include "common.h"
#include "platform/Window.h"

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

		void onResize(int w, int h);

		Sharedref<Platform::Window> window;

		int width = 1280;
		int height = 720;

		bool mFrameBufferResized = false;

	};
}