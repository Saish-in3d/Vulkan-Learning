#pragma once
#include "vk_common.h"
#include "DataBuffer.h"

namespace VKFW::vulkancore {

    class UniformParameter {
    public:
        size_t             mSize{ 0 };
        uint32_t           mBinding{ 0 };
        uint32_t           mCount{ 1 };
        VkDescriptorType   mDescriptorType{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER };
        VkShaderStageFlags mStageFlags{ VK_SHADER_STAGE_ALL };

        std::vector<Sharedref<DataBuffer>> mBuffers{};
    };

}