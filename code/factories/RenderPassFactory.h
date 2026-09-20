#pragma once

#include "../vulkancore/Device.h"
#include "../vulkancore/RenderPass.h"

namespace VKFW::factories {

	class RenderPassFactory
	{
	public: 
		static Sharedref<vulkancore::RenderPass> CreateSimpleRenderPass(const Sharedref<vulkancore::Device>& device, VkFormat colorFormat);
		static Sharedref<vulkancore::RenderPass> CreateWithDepthRenderPass(const Sharedref<vulkancore::Device>& device,VkFormat colorFormat,VkFormat depthFormat = VK_FORMAT_D24_UNORM_S8_UINT);

	};


}