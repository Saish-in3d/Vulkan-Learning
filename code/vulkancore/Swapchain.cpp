#include "SwapCHAIN.h"
#include <algorithm>


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

		SwapChainSupportInfo info;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mDevice->getPhysicalDevice(), mSurface->GetSurface(), &info.mCapablities);

		uint32_t count = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(mDevice->getPhysicalDevice(), mSurface->GetSurface(), &count, nullptr);

		if (count != 0)
		{
			info.mFormats.resize(count);
			vkGetPhysicalDeviceSurfaceFormatsKHR(mDevice->getPhysicalDevice(), mSurface->GetSurface(), &count, info.mFormats.data());
		}

		vkGetPhysicalDeviceSurfacePresentModesKHR(mDevice->getPhysicalDevice(), mSurface->GetSurface(), &count, nullptr);
		if (count != 0)
		{
			info.mPresentModes.resize(count);
			vkGetPhysicalDeviceSurfacePresentModesKHR(mDevice->getPhysicalDevice(), mSurface->GetSurface(), &count, info.mPresentModes.data());
		}
		return info;
	}

	VkSurfaceFormatKHR SwapChain::chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> formats)
	{
		for (const auto& f : formats)
		{
			if (f.format == VK_FORMAT_B8G8R8A8_UNORM && f.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
			{
				return f;
			}
		}


		return formats[0];
	}

	VkPresentModeKHR SwapChain::choosePresentMode(const std::vector<VkPresentModeKHR>& modes)
	{
		for (const auto& m : modes)
		{
			if (m == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				return m;
			}
		}


		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D SwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR capabilities)
	{

		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			return capabilities.currentExtent;
		}

		int w = 0, h = 0;
		glfwGetFramebufferSize(mWindow->GetWindow(), &w, &h);

		VkExtent2D extent = { static_cast<uint32_t>(w),static_cast<uint32_t>(h)};

		extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return extent;
	}

	VkImageView SwapChain::createImageView(VkImage image, VkFormat foarmat, VkImageAspectFlags aspectFlags, uint32_t mipLevels)
	{
		return VkImageView();
	}

}