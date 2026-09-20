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

        void bindVertexBuffer(const std::vector<VkBuffer>& buffers, uint32_t binding = 0, std::vector<VkDeviceSize> offsets = { 0 });
        void bindIndexBuffer(VkBuffer buffer, uint32_t offset = 0, VkIndexType indexType = VK_INDEX_TYPE_UINT32);


        void draw(uint32_t vertexCount);
        void drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance);


        void bindDescriptorSets(const Sharedref<Pipeline>& pipeline, VkDescriptorSet descriptorSet);

        void copyBufferToBuffer(const VkBuffer& src, const VkBuffer& dst,
            uint32_t count,
            const std::vector<VkBufferCopy>& regions);

        void copyBufferToImage(VkBuffer src, VkImage dst,
            VkImageLayout dstLayout,
            uint32_t width, uint32_t height);

        void submitCommandBuffer(VkQueue queue);
        void waitCommandBuffer(VkQueue queue);

    private:
        VkCommandBuffer    mCommandBuffer{ VK_NULL_HANDLE };
        Sharedref<CommandPool>   mCommandPool{ nullptr };
        Sharedref<Device>        mDevice{ nullptr };
    };

}