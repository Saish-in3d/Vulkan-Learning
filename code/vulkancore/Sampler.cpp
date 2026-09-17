#include "Sampler.h"

namespace VKFW::vulkancore {

    Sampler::Sampler(const Sharedref<Device>& device, bool isRepeat)
        : mDevice(device)
    {
        auto mode = isRepeat
            ? VK_SAMPLER_ADDRESS_MODE_REPEAT
            : VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        createSampler(VK_FILTER_LINEAR, VK_FILTER_LINEAR, mode);
    }

    Sampler::~Sampler() 
    {
        if (mSampler != VK_NULL_HANDLE)
            vkDestroySampler(mDevice->getDevice(), mSampler, nullptr);
    }

    void Sampler::createSampler(VkFilter mag, VkFilter min,
        VkSamplerAddressMode addressMode)
    {
        VkSamplerCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        ci.magFilter = mag;
        ci.minFilter = min;
        ci.addressModeU = addressMode;
        ci.addressModeV = addressMode;
        ci.addressModeW = addressMode;

        ci.anisotropyEnable = VK_TRUE;
        ci.maxAnisotropy = 16.0f;

        ci.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        ci.unnormalizedCoordinates = VK_FALSE;
        ci.compareEnable = VK_FALSE;
        ci.compareOp = VK_COMPARE_OP_ALWAYS;

        ci.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        ci.mipLodBias = 0.0f;
        ci.minLod = 0.0f;
        ci.maxLod = 0.0f;
        

        if (vkCreateSampler(mDevice->getDevice(), &ci, nullptr, &mSampler) != VK_SUCCESS)
            throw std::runtime_error("failed to create sampler");
    }

}