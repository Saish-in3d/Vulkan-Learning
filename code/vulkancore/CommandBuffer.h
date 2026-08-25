#pragma once
#include "vk_common.h"
#include "device.h"
#include "commandPool.h"
#include "pipeline.h"

namespace VKFW::vulkancore {

    class CommandBuffer {
    public:
        CommandBuffer(const Sharedref<Device>& device, const Sharedref<CommandPool>& commandPool);
        ~CommandBuffer();

        [[nodiscard]] VkCommandBuffer getCommandBuffer() const { return mCommandBuffer; }

        void beginCommandBuffer(VkCommandBufferUsageFlags flags = 0);
        void endCommandBuffer();

        void beginRenderPass(const VkRenderPassBeginInfo& rpInfo,
            VkSubpassContents contents = VK_SUBPASS_CONTENTS_INLINE);
        void endRenderPass();

        void bindGraphicPipeline(const Sharedref<Pipeline>& pipeline);
        void draw(uint32_t vertexCount);


    private:
        VkCommandBuffer    mCommandBuffer{ VK_NULL_HANDLE };
        Sharedref<CommandPool>   mCommandPool{ nullptr };
        Sharedref<Device>        mDevice{ nullptr };
    };

}