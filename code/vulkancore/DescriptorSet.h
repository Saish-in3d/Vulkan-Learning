#pragma once
#include "vk_common.h"
#include "device.h"
#include "UniformParameter.h"
#include "DescriptorSetLayout.h"
#include "DescriptorPool.h"

namespace VKFW::vulkancore {

    class DescriptorSet {
    public:
        DescriptorSet(const Sharedref<Device>& device,
            const std::vector<Sharedref<UniformParameter>>& params,
            const Sharedref<DescriptorSetLayout>& layout,
            const Sharedref<DescriptorPool>& pool,
            int frameCount);
        ~DescriptorSet() = default;

        [[nodiscard]] VkDescriptorSet getDescriptorSet(int frameIndex) const {
            return mDescriptorSets[frameIndex];
        }

    private:
        std::vector<VkDescriptorSet> mDescriptorSets{};
        Sharedref<Device>                  mDevice{ nullptr };
    };

}