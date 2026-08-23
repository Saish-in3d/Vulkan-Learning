#pragma once
#include "common.h"
#include "platform/Window.h"
#include "vulkancore/instance.h"
#include "vulkancore/WindowSurface.h"
#include "vulkancore/Device.h"
#include "vulkancore/Commandpool.h"
#include "vulkancore/SwapChain.h"
#include "vulkancore/RenderPass.h"

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

		Sharedref<vulkancore::RenderPass> createRenderPass();

		void drawFrame();

		void onResize(int w, int h);

		Sharedref<Platform::Window> mwindow{nullptr};
		Sharedref<vulkancore::Instance> mInstance{ nullptr };
		Sharedref<vulkancore::WindowSurface> mSurface{ nullptr };
		Sharedref<vulkancore::Device> mDevice{ nullptr };
		Sharedref<vulkancore::CommandPool> mCommandPool{ nullptr };
		Sharedref<vulkancore::SwapChain> mSwapChain{ nullptr };
		Sharedref<vulkancore::RenderPass> mRenderPass{ nullptr };

		int width = 1280;
		int height = 720;

		bool mFrameBufferResized = false;

	};
}