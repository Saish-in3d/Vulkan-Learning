#pragma once
#include "vk_common.h"
#include "device.h"

namespace VKFW::vulkancore {

    class Semaphore {
    public:
        Semaphore(const Sharedref<Device>& device);
        ~Semaphore();

        [[nodiscard]] VkSemaphore getSemaphore() const { return mSemaphore; }

    private:
        VkSemaphore      mSemaphore{ VK_NULL_HANDLE };
        Sharedref<Device>      mDevice{ nullptr };
    };

}