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

        void bindDescriptorSets(const Sharedref<Pipeline>& pipeline, VkDescriptorSet descriptorSet);

        void copyBufferToBuffer(const VkBuffer& src, const VkBuffer& dst,
            uint32_t count,
            const std::vector<VkBufferCopy>& regions);
        void submitCommandBuffer(VkQueue queue);
        void waitCommandBuffer(VkQueue queue);

    private:
        VkCommandBuffer    mCommandBuffer{ VK_NULL_HANDLE };
        Sharedref<CommandPool>   mCommandPool{ nullptr };
        Sharedref<Device>        mDevice{ nullptr };
    };

}