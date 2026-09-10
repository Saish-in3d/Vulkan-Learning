#pragma once
#include "vk_common.h"
#include "Device.h"
#include "UniformParameter.h"

namespace VKFW::vulkancore {

    class DescriptorPool {
    public:
        DescriptorPool(const Sharedref<Device>& device);
        ~DescriptorPool();

        void build(const std::vector<Sharedref<UniformParameter>>& params, int frameCount); // frames that are submitted but not completed (in rendering state and not available or presenting)
                                                                                            // MAX_FRAMES_IN_FLIGHT  amount fixed at begnining

        [[nodiscard]] VkDescriptorPool getDescriptorPool() const { return mPool; }

    private:
        void destroyPool();

        VkDescriptorPool mPool{ VK_NULL_HANDLE };
        Sharedref<Device>      mDevice{ nullptr };
        
    };

}