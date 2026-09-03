#pragma once
#include "vk_common.h"
#include "device.h"
#include "commandBuffer.h"
#include "commandPool.h"

namespace VKFW::vulkancore {

    class DataBuffer {
    public:
        static Sharedref<DataBuffer> createVertexBuffer(
            const Sharedref<Device>& device,
            VkDeviceSize size,
            void* pData);

        static Sharedref<DataBuffer> createStageBuffer(
            const Sharedref<Device>& device,
            VkDeviceSize size,
            void* pData = nullptr);


        DataBuffer(const Sharedref<Device>& device,
            VkDeviceSize size,
            VkBufferUsageFlags usage,
            VkMemoryPropertyFlags properties);
        ~DataBuffer();

        void updateBufferByMap(const void* data, VkDeviceSize size);
        void updateBufferByStage(void* data, VkDeviceSize size);

        [[nodiscard]] VkBuffer getBuffer() const { return mBuffer; }
        [[nodiscard]] VkDeviceMemory getMemory() const { return mMemory; }

    private:
        void copyBuffer(const VkBuffer& src, const VkBuffer& dst, VkDeviceSize size);
        static uint32_t findMemoryType(
            const Sharedref<Device>& device,
            uint32_t typeFilter,
            VkMemoryPropertyFlags properties);

        VkBuffer       mBuffer{ VK_NULL_HANDLE };
        VkDeviceMemory mMemory{ VK_NULL_HANDLE };
        Sharedref<Device>    mDevice{ nullptr };
    };

}