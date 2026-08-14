#include "SwapCHAIN.h"
#include <algorithm>


namespace VKFW::vulkancore {


	SwapChain::SwapChain(const Sharedref<Device>& device, const Sharedref<Platform::Window>& window, const Sharedref<WindowSurface>& surface)
		:mDevice(device), mWindow(window), mSurface(surface)
	{

		auto info = querySwapChainSupportInfo();
		auto format = chooseSurfaceFormat(info.mFormats);
		auto presentMode = choosePresentMode(info.mPresentModes);
		auto extent = chooseSwapExtent(info.mCapablities);

		std::cout << "Choosen format: " << format.format << " Chosen present mode: " << presentMode
			<< " Extent: " << extent.width << " x" << extent.height << std::endl;

		//Making sure mimage count is within our capabilities
		mImageCount = info.mCapablities.minImageCount + 1;
		if (info.mCapablities.maxImageCount > 0 && mImageCount > info.mCapablities.maxImageCount)
		{
			mImageCount = info.mCapablities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR ci{};
		ci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		ci.surface = mSurface->GetSurface();
		ci.minImageCount = mImageCount;
		ci.imageColorSpace = format.colorSpace;
		ci.imageFormat = format.format;
		ci.imageExtent = extent;
		ci.imageArrayLayers = 1;
		ci.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		//Setting image sharing mode (Totally based on physical device capablities, driver confif nad other factors
		//Swapchain images are accessed/touched by two entities: GraphicsQueue and PresentQueue.They belong to families
		// In most cases they belong to same QueueFamily, so there is no issue of image ownership
		//but in some cases (hardware config or some igpu) it's possible that both can have different queue family
		//For this we need to setup the setup mode based on our requirements
		std::vector<uint32_t> indices{
			mDevice->GetGraphicQueueFamily().value(),
			mDevice->GetPresentQueueFamily().value()
		};

		if (indices[0] == indices[1]) //Belong to same queue family
		{
			ci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			ci.queueFamilyIndexCount = 0;
			ci.pQueueFamilyIndices = nullptr;
		}
		else
		{
			ci.imageSharingMode = VK_SHARING_MODE_CONCURRENT;//Sharing ownership mode
			ci.queueFamilyIndexCount = static_cast<uint32_t>(indices.size());
			ci.pQueueFamilyIndices = indices.data();
		}
		ci.preTransform = info.mCapablities.currentTransform;//Transform of surface (Rotation of surface landscape/potrait/custom)
		ci.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		ci.presentMode = presentMode;
		ci.clipped = VK_TRUE;//Wont render pixels if thap particular part is occluded by other app/window

		VkResult r = vkCreateSwapchainKHR(mDevice->getDevice(), &ci, nullptr,&mSwapChain);
		if (r != VK_SUCCESS)
		{
			throw std::runtime_error("vkCreateSwapchainKHR failed");
		}
		mSwapChainFormat = format.format;
		mSwapChainExtent = extent;

		vkGetSwapchainImagesKHR(mDevice->getDevice(), mSwapChain, &mImageCount, nullptr);
		mSwapChainImages.resize(mImageCount);
		vkGetSwapchainImagesKHR(mDevice->getDevice(), mSwapChain, &mImageCount, mSwapChainImages.data());


		std::cout << "SwapChainCreated: " << format.format << " Images count: " << mImageCount
			<< " Extent: " << extent.width << " x" << extent.height << std::endl;


		mSwapChainImageViews.resize(mImageCount);
		for (size_t i = 0; i < mImageCount; i++)
		{
			mSwapChainImageViews[i] = createImageView(mSwapChainImages[i], mSwapChainFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
		}

	}

	SwapChain::~SwapChain()
	{
		for (auto fb : mSwapChainFramebuffers)
		{
			vkDestroyFramebuffer(mDevice->getDevice(), fb, nullptr);
		}

		for (auto iv : mSwapChainImageViews)
		{
			vkDestroyImageView(mDevice->getDevice(), iv, nullptr);
		}

		if (mSwapChain != VK_NULL_HANDLE)
		{
			vkDestroySwapchainKHR(mDevice->getDevice(), mSwapChain, nullptr);
		}

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
		VkImageViewCreateInfo ci{};
		ci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		ci.format = foarmat;
		ci.image = image;
		ci.viewType = VK_IMAGE_VIEW_TYPE_2D;
		
		ci.subresourceRange.aspectMask = aspectFlags;
		ci.subresourceRange.baseMipLevel = 0;
		ci.subresourceRange.levelCount = mipLevels;
		ci.subresourceRange.baseArrayLayer = 0;
		ci.subresourceRange.layerCount = 1;

		VkImageView view{VK_NULL_HANDLE};
		if (vkCreateImageView(mDevice->getDevice(), &ci, nullptr, &view) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create image");
		}

		return view;

	}

}