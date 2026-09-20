#pragma once
#include "vk_common.h"
#include "device.h"
#include "../platform/window.h"
#include "windowSurface.h"
#include "image.h"
#include "Commandpool.h"


namespace VKFW::vulkancore {

	class RenderPass; //forward declaration

}

namespace VKFW::vulkancore {

	struct SwapChainSupportInfo
	{
		VkSurfaceCapabilitiesKHR mCapablities{};
		std::vector<VkSurfaceFormatKHR> mFormats;
		std::vector<VkPresentModeKHR> mPresentModes;
	};

	class SwapChain
	{

	public:

		SwapChain(const Sharedref<Device>& device,
			const Sharedref<Platform::Window>& window,
			const Sharedref<WindowSurface>& surface,
			const Sharedref<CommandPool>& commandPool);

		~SwapChain();


		void createFrameBuffers(const Sharedref<RenderPass>& renderPass);

		SwapChainSupportInfo querySwapChainSupportInfo();
		VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> formats);
		VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR>& modes);
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR capabilities);

		[[nodiscard]] VkFormat                      getSwapChainImageFormat()    const { return mSwapChainFormat; }
		[[nodiscard]] VkExtent2D                    getSwapChainExtent()         const { return mSwapChainExtent; }
		[[nodiscard]] VkSwapchainKHR                getSwapChain()               const { return mSwapChain; }
		[[nodiscard]] uint32_t                      getImageCount()              const { return mImageCount; }
		[[nodiscard]] std::vector<VkImage>          getSwapChainImages()         const { return mSwapChainImages; }
		[[nodiscard]] std::vector<VkImageView>      getSwapChainImageViews()     const { return mSwapChainImageViews; }
		[[nodiscard]] std::vector<VkFramebuffer>    getSwapChainFramebuffers()   const { return mSwapChainFramebuffers; }


		uint32_t acquireNextImage(VkSemaphore semaphore, VkFence fence = VK_NULL_HANDLE)
		{
			uint32_t imageIndex = 0;
			vkAcquireNextImageKHR(mDevice->getDevice(),
				mSwapChain,
				std::numeric_limits<uint64_t>::max(),
				semaphore,
				fence,
				&imageIndex);


			return imageIndex;


		}



	private:

		VkImageView createImageView(VkImage image, VkFormat foarmat, VkImageAspectFlags aspectFlags, uint32_t mipLevels = 1);

		Sharedref<Device>           mDevice{ nullptr };
		Sharedref<Platform::Window> mWindow{ nullptr };
		Sharedref<WindowSurface>    mSurface{ nullptr };
		Sharedref<CommandPool>      mCommandPool{ nullptr };


		VkSwapchainKHR mSwapChain{ VK_NULL_HANDLE };
		VkFormat       mSwapChainFormat{};
		VkExtent2D     mSwapChainExtent{};
		uint32_t       mImageCount{ 0 };

		std::vector<VkImage>       mSwapChainImages{};
		std::vector<Sharedref<vulkancore::Image>> mDepthImages{};
		std::vector<VkImageView>   mSwapChainImageViews{};
		std::vector<VkFramebuffer> mSwapChainFramebuffers{};


	};


}