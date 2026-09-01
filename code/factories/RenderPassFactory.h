#pragma once

#include "../vulkancore/Device.h"
#include "../vulkancore/RenderPass.h"

namespace VKFW::factories {

	class RenderPassFactory
	{
	public: 
		static Sharedref<vulkancore::RenderPass> CreateSimpleRenderPass(const Sharedref<vulkancore::Device>& device, VkFormat colorFormat);


	};


}