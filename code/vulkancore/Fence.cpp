#include "Fence.h"

VKFW::vulkancore::Fence::Fence(const Sharedref<Device>& device, bool signaled)
	: mDevice(device)
{
    VkFenceCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    ci.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

    if (vkCreateFence(mDevice->getDevice(), &ci, nullptr, &mFence) != VK_SUCCESS)
        throw std::runtime_error("failed to create fence");
}

VKFW::vulkancore::Fence::~Fence()
{
    if (mFence != VK_NULL_HANDLE)
        vkDestroyFence(mDevice->getDevice(), mFence, nullptr);
}

void VKFW::vulkancore::Fence::waitForFence(uint64_t timeout)
{
    vkWaitForFences(mDevice->getDevice(), 1, &mFence, VK_TRUE, timeout);
}

void VKFW::vulkancore::Fence::resetFence()
{
    vkResetFences(mDevice->getDevice(), 1, &mFence);
}
