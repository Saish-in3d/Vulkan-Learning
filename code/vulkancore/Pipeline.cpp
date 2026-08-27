#include "Pipeline.h"

namespace VKFW::vulkancore {

    Pipeline::Pipeline(const Sharedref<Device>& device, const Sharedref<RenderPass>& renderPass)
        : mDevice(device), mRenderPass(renderPass) 
    {
    }

    Pipeline::~Pipeline() {
        if (mLayout != VK_NULL_HANDLE)
            vkDestroyPipelineLayout(mDevice->getDevice(), mLayout, nullptr);
        if (mPipeline != VK_NULL_HANDLE)
            vkDestroyPipeline(mDevice->getDevice(), mPipeline, nullptr);
    }

    void Pipeline::setShaderGroup(const std::vector<Sharedref<Shader>>& shaders)
    {
        mShaders = shaders;
    }

    void Pipeline::pushBlendAttachment(const VkPipelineColorBlendAttachmentState& state) 
    {
        mBlendAttachmentStates.push_back(state);
    }

    void Pipeline::build() 
    {
        std::vector<VkPipelineShaderStageCreateInfo> stages;
        for (const auto& shader : mShaders)
        {
            VkPipelineShaderStageCreateInfo s{};
            s.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            s.stage = shader->getShaderStage();
            s.module = shader->getShaderModule();
            s.pName = shader->getEntryPoint().c_str();

            stages.push_back(s);

        }

        mBlendState.attachmentCount = static_cast<uint32_t>(mBlendAttachmentStates.size());
        mBlendState.pAttachments = mBlendAttachmentStates.data();

        mDynamicState.dynamicStateCount = static_cast<uint32_t>(mDynamicStates.size());
        mDynamicState.pDynamicStates = mDynamicStates.data();

        if (mLayout != VK_NULL_HANDLE)
        {
            vkDestroyPipelineLayout(mDevice->getDevice(), mLayout, nullptr);
            mLayout = VK_NULL_HANDLE;

        }

        if (vkCreatePipelineLayout(mDevice->getDevice(), &mPipelineLayoutInfo, nullptr, &mLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to vkCreatePipelineLayout");
        }

        VkGraphicsPipelineCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        ci.stageCount = static_cast<uint32_t>(stages.size());
        ci.pStages = stages.data();
        ci.pVertexInputState = &mVertexInputState;
        ci.pInputAssemblyState = &mAssemblyState;
        ci.pViewportState = &mViewportState;
        ci.pRasterizationState = &mRasterState;
        ci.pMultisampleState = &mMultisampleState;
        ci.pColorBlendState = &mBlendState;
        ci.pDynamicState = &mDynamicState;
        ci.pDepthStencilState = &mDepthStencilState;
        ci.layout = mLayout;
        ci.renderPass = mRenderPass->getRenderPass();
        ci.subpass = 0;
        ci.basePipelineIndex = -1;


        if (mPipeline != VK_NULL_HANDLE)
        {
            vkDestroyPipeline(mDevice->getDevice(), mPipeline, nullptr);
            mPipeline = VK_NULL_HANDLE;
        }

        if(vkCreateGraphicsPipelines(mDevice->getDevice(), VK_NULL_HANDLE, 1, &ci, nullptr, &mPipeline)!= VK_SUCCESS)
        {
            throw std::runtime_error("failed to vkCreateGraphicsPipelines");
        }
    }

}