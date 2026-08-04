#pragma once
#include "vk_common.h"
#include "device.h"

namespace VKFW::vulkancore {

    class CommandPool {
    public:
        CommandPool(const Sharedref<Device> device, VkCommandPoolCreateFlagBits = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

        ~CommandPool();

        [[nodiscard]] VkCommandPool getCommandPool() const { return mCommandPool; }

    private:
        VkCommandPool mCommandPool{ VK_NULL_HANDLE };
        Sharedref<Device>   mDevice{ nullptr };
    };

} // namespace VKFW::vulkancore