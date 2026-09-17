#include "DescriptorSetLayout.h"

VKFW::vulkancore::DescriptorSetLayout::DescriptorSetLayout(const Sharedref<Device>& device)
	:mDevice(device)
{

}

VKFW::vulkancore::DescriptorSetLayout::~DescriptorSetLayout()
{
	if (mLayout != VK_NULL_HANDLE)
		vkDestroyDescriptorSetLayout(mDevice->getDevice(), mLayout, nullptr);
}

void VKFW::vulkancore::DescriptorSetLayout::build(const std::vector<Sharedref<DescriptorParameter>>& params) // current length of this array is 1 as only single UBO
{
    mBindingParameters = params;

    if (mLayout != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(mDevice->getDevice(), mLayout, nullptr);
        mLayout = VK_NULL_HANDLE;
    }

    std::vector<VkDescriptorSetLayoutBinding> bindings;
    for (const auto& param : params) {
        VkDescriptorSetLayoutBinding b{};
        b.binding = param->mBinding; //0
        b.descriptorType = param->mDescriptorType; //UBO
        b.descriptorCount = param->mCount; //descriptor count (1 IN THIS CASE only for cameraUBO 2 if camera+ model)
        b.stageFlags = param->mStageFlags; //bind to which stage of rendering pipeline
        b.pImmutableSamplers = nullptr; //later for image sampler
        bindings.push_back(b);
    }

    VkDescriptorSetLayoutCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    ci.bindingCount = (uint32_t)bindings.size();
    ci.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(mDevice->getDevice(), &ci, nullptr, &mLayout) != VK_SUCCESS)
        throw std::runtime_error("failed to create descriptor set layout");
}
