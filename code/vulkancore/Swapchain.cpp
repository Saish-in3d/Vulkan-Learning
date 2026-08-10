#include "SwapCHAIN.h"



namespace VKFW::vulkancore {











	SwapChain::SwapChain(const Sharedref<Device>& device, const Sharedref<Platform::Window>& window, const Sharedref<WindowSurface>& surface)
	{
	}

	SwapChain::~SwapChain()
	{
	}

	void SwapChain::createFrameBuffers(const Sharedref<renderpass>& renderPass)
	{
	}

	SwapChainSupportInfo SwapChain::querySwapChainSupportInfo()
	{
		return SwapChainSupportInfo();
	}

	VkSurfaceFormatKHR SwapChain::chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> formats)
	{
		return VkSurfaceFormatKHR();
	}

	VkPresentModeKHR SwapChain::choosePresentMode(const std::vector<VkPresentModeKHR>& modes)
	{
		return VkPresentModeKHR();
	}

	VkExtent2D SwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR capabilities)
	{
		return VkExtent2D();
	}

	VkImageView SwapChain::createImageView(VkImage image, VkFormat foarmat, VkImageAspectFlags aspectFlags, uint32_t mipLevels)
	{
		return VkImageView();
	}

}