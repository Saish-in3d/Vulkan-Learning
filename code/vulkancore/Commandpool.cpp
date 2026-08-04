#include "commandPool.h"
#include <stdexcept>

namespace VKFW::vulkancore {

    

    CommandPool::CommandPool(const Sharedref<Device> device, VkCommandPoolCreateFlagBits flags) :mDevice(device)
    {
        VkCommandPoolCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        ci.queueFamilyIndex = mDevice->GetGraphicQueueFamily().value();
        ci.flags = flags;

        if (vkCreateCommandPool(mDevice->getDevice(), &ci, nullptr, &mCommandPool) != VK_SUCCESS)
        {
            throw std::runtime_error("Graphic command pool creation failed");
        }
    }

    CommandPool::~CommandPool()
    {
        if (mCommandPool != VK_NULL_HANDLE)
        {
            vkDestroyCommandPool(mDevice->getDevice(), mCommandPool, nullptr);
        }

    }

} // namespace VKFW::vulkancore
