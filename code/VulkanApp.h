#pragma once
#include "common.h"
#include "platform/Window.h"
#include "vulkancore/instance.h"
#include "vulkancore/WindowSurface.h"
#include "vulkancore/Device.h"
#include "vulkancore/Commandpool.h"
#include "vulkancore/SwapChain.h"
#include "vulkancore/RenderPass.h"
#include "vulkancore/Pipeline.h"
#include "vulkancore/Commandbuffer.h"
#include "factories/RenderPassFactory.h"
#include "vulkancore/Semaphore.h"
#include "vulkancore/Fence.h"
#include "vulkancore/DataBuffer.h"
#include "renderer/DescriptorManager.h"

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


		Sharedref<vulkancore::Pipeline> VulkanApp::createPipeline();

		void drawFrame();

		void recordCommandBuffers();

		void onResize(int w, int h);

		float getFrameTime();

		void updateUBO(float dt);



		void createSyncObjects();

		Sharedref<Platform::Window> mwindow{ nullptr };
		Sharedref<vulkancore::Instance> mInstance{ nullptr };
		Sharedref<vulkancore::WindowSurface> mSurface{ nullptr };
		Sharedref<vulkancore::Device> mDevice{ nullptr };
		Sharedref<vulkancore::CommandPool> mCommandPool{ nullptr };
		Sharedref<vulkancore::SwapChain> mSwapChain{ nullptr };
		Sharedref<vulkancore::RenderPass> mRenderPass{ nullptr };
		Sharedref<vulkancore::Pipeline> mPipeline{ nullptr };
		Sharedref<vulkancore::DataBuffer> mVertexBuffer{ nullptr };
		Sharedref<renderer::UniformManager> mUniformManager{ nullptr };

		float mRotatiom = 0.0f;
		double lastTime = 0;

		std::vector<Sharedref<vulkancore::Semaphore>> mImageAvailableSemaphores{};
		std::vector<Sharedref<vulkancore::Semaphore>> mRenderFinishedSemaphores{};
		std::vector<Sharedref<vulkancore::Fence>>     mFences{};
		uint32_t mCurrentFrame{ 0 };
		
		std::vector< Sharedref<vulkancore::CommandBuffer>>mCommandBuffers;

		int width = 1280;
		int height = 720;

		bool mFrameBufferResized = false;


		inline std::filesystem::path exeDir()
		{
			wchar_t buf[MAX_PATH];
			GetModuleFileNameW(nullptr, buf, MAX_PATH);
			return std::filesystem::path(buf).parent_path();
		}

	};
}