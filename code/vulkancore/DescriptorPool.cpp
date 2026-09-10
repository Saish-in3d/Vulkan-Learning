#include "DescriptorPool.h"
#include <stdexcept>

namespace VKFW::vulkancore {

    DescriptorPool::DescriptorPool(const Sharedref<Device>& device)
        : mDevice(device) {
    }

    DescriptorPool::~DescriptorPool() {
        destroyPool();
    }

    void DescriptorPool::build(
        const std::vector<Sharedref<UniformParameter>>& params,
        int frameCount)
    {
        destroyPool();

        // count UBO descriptors
        uint32_t uboCount = 0;
        for (const auto& p : params)
            if (p->mDescriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
                uboCount += p->mCount;
       

        std::vector<VkDescriptorPoolSize> sizes;
        if (uboCount > 0) {
            VkDescriptorPoolSize s{};
            s.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            s.descriptorCount = uboCount * frameCount;
            sizes.push_back(s);
        }

        VkDescriptorPoolCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        ci.poolSizeCount = (uint32_t)sizes.size();
        ci.pPoolSizes = sizes.data();
        ci.maxSets = (uint32_t)frameCount;

        if (vkCreateDescriptorPool(mDevice->getDevice(), &ci, nullptr, &mPool) != VK_SUCCESS)
            throw std::runtime_error("failed to create descriptor pool");
    }

    void DescriptorPool::destroyPool() {
        if (mPool != VK_NULL_HANDLE) {
            vkDestroyDescriptorPool(mDevice->getDevice(), mPool, nullptr);
            mPool = VK_NULL_HANDLE;
        }
    }

}