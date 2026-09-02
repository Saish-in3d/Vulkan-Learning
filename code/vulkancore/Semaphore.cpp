#include "Semaphore.h"

VKFW::vulkancore::Semaphore::Semaphore(const Sharedref<Device>& device)
	:mDevice(device)
{
    VkSemaphoreCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if (vkCreateSemaphore(mDevice->getDevice(), &ci, nullptr, &mSemaphore) != VK_SUCCESS)
        throw std::runtime_error("failed to create semaphore");
}

VKFW::vulkancore::Semaphore::~Semaphore()
{
    if (mSemaphore != VK_NULL_HANDLE)
        vkDestroySemaphore(mDevice->getDevice(), mSemaphore, nullptr);
}
