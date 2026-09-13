#include "CommandBuffer.h"
#include "CommandBuffer.h"
#include "commandBuffer.h"
#include <stdexcept>

namespace VKFW::vulkancore {

    CommandBuffer::CommandBuffer(const Sharedref<Device>& device,
        const Sharedref<CommandPool>& commandPool)
        : mDevice(device), mCommandPool(commandPool)
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = mCommandPool->getCommandPool();
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(mDevice->getDevice(),
            &allocInfo, &mCommandBuffer) != VK_SUCCESS)
            throw std::runtime_error("failed to allocate command buffer");
    }

    CommandBuffer::~CommandBuffer() {
        if (mCommandBuffer != VK_NULL_HANDLE)
            vkFreeCommandBuffers(mDevice->getDevice(),
                mCommandPool->getCommandPool(), 1, &mCommandBuffer);
    }

    void CommandBuffer::beginCommandBuffer(VkCommandBufferUsageFlags flags) {
        VkCommandBufferBeginInfo info{};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        info.flags = flags;
        if (vkBeginCommandBuffer(mCommandBuffer, &info) != VK_SUCCESS)
            throw std::runtime_error("failed to begin command buffer");
    }

    void CommandBuffer::endCommandBuffer() {
        if (vkEndCommandBuffer(mCommandBuffer) != VK_SUCCESS)
            throw std::runtime_error("failed to end command buffer");
    }

    void CommandBuffer::beginRenderPass(const VkRenderPassBeginInfo& rpInfo,
        VkSubpassContents contents) {
        vkCmdBeginRenderPass(mCommandBuffer, &rpInfo, contents);
    }

    void CommandBuffer::endRenderPass() {
        vkCmdEndRenderPass(mCommandBuffer);
    }

    void CommandBuffer::bindGraphicPipeline(const Sharedref<Pipeline>& pipeline) {
        vkCmdBindPipeline(mCommandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getPipeline());
    }

    void CommandBuffer::draw(uint32_t vertexCount) {
        vkCmdDraw(mCommandBuffer, vertexCount, 1, 0, 0);
    }

    void CommandBuffer::bindDescriptorSets(const Sharedref<Pipeline>& pipeline, VkDescriptorSet descriptorSet)
    {
        vkCmdBindDescriptorSets(
            mCommandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipeline->getPipelineLayout(),
            0,
            1,
            &descriptorSet,
            0,
            nullptr
        );

    }

    void CommandBuffer::copyBufferToBuffer(
        const VkBuffer& src, const VkBuffer& dst,
        uint32_t count,
        const std::vector<VkBufferCopy>& regions)
    {
        vkCmdCopyBuffer(mCommandBuffer, src, dst, count, regions.data());
    }

    void CommandBuffer::copyBufferToImage(VkBuffer src, VkImage dst, VkImageLayout dstLayout, uint32_t width, uint32_t height)
    {

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = { 0, 0, 0 };
        region.imageExtent = { width, height, 1 };

        vkCmdCopyBufferToImage(mCommandBuffer, src, dst, dstLayout, 1, &region);

    }

    void CommandBuffer::submitCommandBuffer(VkQueue queue) {
        VkSubmitInfo si{};
        si.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        si.commandBufferCount = 1;
        si.pCommandBuffers = &mCommandBuffer;
        vkQueueSubmit(queue, 1, &si, VK_NULL_HANDLE);
    }

    void CommandBuffer::waitCommandBuffer(VkQueue queue) {
        vkQueueWaitIdle(queue);
    }

}