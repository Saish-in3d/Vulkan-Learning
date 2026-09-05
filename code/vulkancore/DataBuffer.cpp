#include "dataBuffer.h"
#include <stdexcept>
#include <cstring>


namespace VKFW::vulkancore {

    DataBuffer::DataBuffer(const Sharedref<Device>& device,
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties)
        : mDevice(device)
    {
        VkBufferCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        ci.size = size;
        ci.usage = usage;
        ci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(mDevice->getDevice(), &ci, nullptr, &mBuffer) != VK_SUCCESS)
            throw std::runtime_error("failed to create buffer");

        VkMemoryRequirements memReq{};
        vkGetBufferMemoryRequirements(mDevice->getDevice(), mBuffer, &memReq);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memReq.size;
        allocInfo.memoryTypeIndex = findMemoryType(mDevice, memReq.memoryTypeBits, properties);

        if (vkAllocateMemory(mDevice->getDevice(), &allocInfo, nullptr, &mMemory) != VK_SUCCESS)
            throw std::runtime_error("failed to allocate buffer memory");

        vkBindBufferMemory(mDevice->getDevice(), mBuffer, mMemory, 0);
    }

    DataBuffer::~DataBuffer() 
    {
        if (mMemory != VK_NULL_HANDLE)
            vkFreeMemory(mDevice->getDevice(), mMemory, nullptr);
        if (mBuffer != VK_NULL_HANDLE)
            vkDestroyBuffer(mDevice->getDevice(), mBuffer, nullptr);
    }

    Sharedref<DataBuffer> DataBuffer::createVertexBuffer(
        const Sharedref<Device>& device,
        VkDeviceSize size,
        void* pData)
    {
        auto buffer = MakeSharedRef<DataBuffer>(
            device, size,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        buffer->updateBufferByStage(pData, size);
        return buffer;
    }

    Sharedref<DataBuffer> DataBuffer::createStageBuffer(
        const Sharedref<Device>& device,
        VkDeviceSize size,
        void* pData)
    {
        auto buffer = MakeSharedRef<DataBuffer>(
            device, size,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        if (pData != nullptr)
            buffer->updateBufferByMap(pData, size);
        return buffer;
    }

    void DataBuffer::updateBufferByMap(const void* data, VkDeviceSize size)
    {
        void* mapped = nullptr;
        vkMapMemory(mDevice->getDevice(), mMemory, 0, size, 0, &mapped);
        memcpy(mapped, data, static_cast<size_t>(size));
        vkUnmapMemory(mDevice->getDevice(), mMemory);
    }

    void DataBuffer::updateBufferByStage(void* data, VkDeviceSize size) 
    {
        auto staging = DataBuffer::createStageBuffer(mDevice, size, data);
        copyBuffer(staging->getBuffer(), mBuffer, size);
    }

    void DataBuffer::copyBuffer(const VkBuffer& src, const VkBuffer& dst, VkDeviceSize size)
    {
        // use a transient pool for one-time transfer commands
        auto pool = MakeSharedRef<CommandPool>(mDevice,
            VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);
        auto cmd = MakeSharedRef<CommandBuffer>(mDevice, pool);

        cmd->beginCommandBuffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

        VkBufferCopy region{};
        region.size = size;
        cmd->copyBufferToBuffer(src, dst, 1, { region });

        cmd->endCommandBuffer();
        cmd->submitCommandBuffer(mDevice->getGraphicQueue());
        cmd->waitCommandBuffer(mDevice->getGraphicQueue());
    }

    uint32_t DataBuffer::findMemoryType(
        const Sharedref<Device>& device,
        uint32_t typeFilter,
        VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memProp{};
        vkGetPhysicalDeviceMemoryProperties(device->getPhysicalDevice(), &memProp);

        for (uint32_t i = 0; i < memProp.memoryTypeCount; i++) 
        {
            if ((typeFilter & (1u << i)) &&(memProp.memoryTypes[i].propertyFlags & properties) == properties)
                return i;
        }
        throw std::runtime_error("failed to find suitable memory type");
    }

}
