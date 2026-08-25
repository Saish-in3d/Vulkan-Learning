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

}