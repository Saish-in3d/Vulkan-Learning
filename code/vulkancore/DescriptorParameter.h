#pragma once
#include "vk_common.h"
#include "DataBuffer.h"
#include "Texture.h"


namespace VKFW::vulkancore {

    class DescriptorParameter { //can be eatch for cameraUBO, modeltransformUBO. etc
    public:
        size_t             mSize{ 0 };
        uint32_t           mBinding{ 0 };
        uint32_t           mCount{ 1 };
        VkDescriptorType   mDescriptorType{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER };
        VkShaderStageFlags mStageFlags{ VK_SHADER_STAGE_ALL };

        //This will also hold other descriptor parameters like SSBO (Storage buffers), texture samplers, etc

        //VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
        std::vector<Sharedref<DataBuffer>> mBuffers{}; //multiple buffers for multiple swapchain images

        //VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
        std::vector < Sharedref<Texture>> mTextures;
    };

}