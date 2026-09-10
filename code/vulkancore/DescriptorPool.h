#pragma once
#include "vk_common.h"
#include "Device.h"
#include "UniformParameter.h"

namespace VKFW::vulkancore {

    class DescriptorPool {
    public:
        DescriptorPool(const Sharedref<Device>& device);
        ~DescriptorPool();

        void build(const std::vector<Sharedref<UniformParameter>>& params, int frameCount);

        [[nodiscard]] VkDescriptorPool getDescriptorPool() const { return mPool; }

    private:
        void destroyPool();

        VkDescriptorPool mPool{ VK_NULL_HANDLE };
        Sharedref<Device>      mDevice{ nullptr };
        
    };

}