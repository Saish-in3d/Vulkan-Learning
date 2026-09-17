#pragma once
#include "vk_common.h"
#include "device.h"

namespace VKFW::vulkancore {

    class Sampler {
    public:
        Sampler(const Sharedref<Device>& device, bool isRepeat = false);
        ~Sampler();

        [[nodiscard]] VkSampler getSampler() const { return mSampler; }
        
    private:
        void createSampler(VkFilter mag, VkFilter min,
            VkSamplerAddressMode addressMode);

        VkSampler   mSampler{ VK_NULL_HANDLE };
        Sharedref<Device> mDevice{ nullptr };
    };

}