#include "DescriptorSet.h"

namespace VKFW::vulkancore {

    DescriptorSet::DescriptorSet( // Multiple descriptor sets for each frame on fly //Each descriptor set contains multiple descriptors
        const Sharedref<Device>& device,
        const std::vector<Sharedref<UniformParameter>>& params,
        const Sharedref<DescriptorSetLayout>& layout,
        const Sharedref<DescriptorPool>& pool,
        int frameCount)
        : mDevice(device)
    {
        mDescriptorSets.resize(frameCount);
        std::vector<VkDescriptorSetLayout> layouts(frameCount, layout->getLayout());

        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = pool->getDescriptorPool();
        allocInfo.descriptorSetCount = (uint32_t)frameCount;
        allocInfo.pSetLayouts = layouts.data();

        //Actually Allocating descriptor sets to descriptor pool
        if (vkAllocateDescriptorSets(mDevice->getDevice(),
            &allocInfo, mDescriptorSets.data()) != VK_SUCCESS)
            throw std::runtime_error("failed to allocate descriptor sets");

        // write/bind/assign/upddating UBO descriptors for each frame on the fly via std::vector<VkWriteDescriptorSet> writes;
        for (int i = 0; i < frameCount; i++) {
            std::vector<VkWriteDescriptorSet> writes;

            for (const auto& param : params) {
                if (param->mDescriptorType != VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
                    continue; //Will  add other types of descriptors later

                VkWriteDescriptorSet w{};
                w.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                w.dstSet = mDescriptorSets[i];
                w.dstBinding = param->mBinding;
                w.dstArrayElement = 0;
                w.descriptorType = param->mDescriptorType;
                w.descriptorCount = param->mCount;
                w.pBufferInfo = &param->mBuffers[i]->getBufferInfo();
                writes.push_back(w);
            }

            //Actually write/bind/assign/upddating UBO descriptors for each frame on the fly for first frame
            vkUpdateDescriptorSets(mDevice->getDevice(),
                (uint32_t)writes.size(), writes.data(), 0, nullptr);
        }
    }

}