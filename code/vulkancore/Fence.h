#pragma once
#include "vk_common.h"
#include "device.h"

namespace VKFW::vulkancore {

    class Fence {
    public:
        Fence(const Sharedref<Device>& device, bool signaled = true);
        ~Fence();

        void waitForFence(uint64_t timeout = UINT64_MAX);
        void resetFence();

        [[nodiscard]] VkFence getFence() const { return mFence; }

    private:
        VkFence     mFence{ VK_NULL_HANDLE };
        Sharedref<Device> mDevice{ nullptr };
    };

}