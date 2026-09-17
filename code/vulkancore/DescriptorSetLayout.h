#pragma once
#include "vk_common.h"
#include "device.h"
#include "DescriptorParameter.h"

namespace VKFW::vulkancore {

    class DescriptorSetLayout {
    public:
        DescriptorSetLayout(const Sharedref<Device>& device);
        ~DescriptorSetLayout();

        void build(const std::vector<Sharedref<DescriptorParameter>>& params);

        [[nodiscard]] VkDescriptorSetLayout getLayout() const { return mLayout; }
        [[nodiscard]] Sharedref<Device>           getDevice() const { return mDevice; }
        [[nodiscard]] std::vector<Sharedref<DescriptorParameter>> getBindingParameters() const { return mBindingParameters; }

    private:
        VkDescriptorSetLayout              mLayout{ VK_NULL_HANDLE };
        Sharedref<Device>                        mDevice{ nullptr };
        std::vector<Sharedref<DescriptorParameter>> mBindingParameters{};
    };

}