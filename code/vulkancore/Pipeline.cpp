#include "Pipeline.h"

namespace VKFW::vulkancore {

    Pipeline::Pipeline(const Sharedref<Device>& device, const Sharedref<RenderPass>& renderPass)
        : mDevice(device), mRenderPass(renderPass) {
    }

    Pipeline::~Pipeline() {
        if (mLayout != VK_NULL_HANDLE)
            vkDestroyPipelineLayout(mDevice->getDevice(), mLayout, nullptr);
        if (mPipeline != VK_NULL_HANDLE)
            vkDestroyPipeline(mDevice->getDevice(), mPipeline, nullptr);
    }

    void Pipeline::setShaderGroup(const std::vector<Sharedref<Shader>>& shaders) {
        mShaders = shaders;
    }

    void Pipeline::pushBlendAttachment(const VkPipelineColorBlendAttachmentState& state) {
        mBlendAttachmentStates.push_back(state);
    }

    void Pipeline::build() {
        
    }

}