#pragma once
#include "vk_common.h"
#include "device.h"
#include "shader.h"
#include "renderPass.h"

namespace VKFW::vulkancore {

    class Pipeline {
    public:
        Pipeline(const Sharedref<Device>& device, const Sharedref<RenderPass>& renderPass);
        ~Pipeline();

        [[nodiscard]] VkPipeline       getPipeline()       const { return mPipeline; }
        [[nodiscard]] VkPipelineLayout getPipelineLayout() const { return mLayout; }

        void setShaderGroup(const std::vector<Sharedref<Shader>>& shaders);
        void pushBlendAttachment(const VkPipelineColorBlendAttachmentState& state);
        void build();

    public:
        
        VkPipelineVertexInputStateCreateInfo   mVertexInputState{};
        VkPipelineInputAssemblyStateCreateInfo mAssemblyState{};
        VkPipelineViewportStateCreateInfo      mViewportState{};
        VkPipelineRasterizationStateCreateInfo mRasterState{};
        VkPipelineDynamicStateCreateInfo       mDynamicState{};
        VkPipelineMultisampleStateCreateInfo   mMultisampleState{};
        VkPipelineColorBlendStateCreateInfo    mBlendState{};
        VkPipelineDepthStencilStateCreateInfo  mDepthStencilState{};
        VkPipelineLayoutCreateInfo             mPipelineLayoutInfo{};

        std::vector<VkPipelineColorBlendAttachmentState> mBlendAttachmentStates{};
        std::vector<VkDynamicState>                      mDynamicStates{};
        

    private:
        VkPipeline       mPipeline{ VK_NULL_HANDLE };
        VkPipelineLayout mLayout{ VK_NULL_HANDLE };
        Sharedref<Device>      mDevice{ nullptr };
        Sharedref<RenderPass>  mRenderPass{ nullptr };
        std::vector<Sharedref<Shader>> mShaders{};
    };

}