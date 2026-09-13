#pragma once
#include "vk_common.h"
#include "device.h"
#include "commandPool.h"
#include "commandBuffer.h"
#include "DataBuffer.h"

namespace VKFW::vulkancore {

    struct ImageDescription {
        uint32_t              width{ 0 };
        uint32_t              height{ 0 };
        uint32_t              depth{ 1 };
        uint32_t              mipLevels{ 1 };
        uint32_t              arrayLayers{ 1 };
        VkFormat              format{ VK_FORMAT_UNDEFINED };
        VkImageType           imageType{ VK_IMAGE_TYPE_2D };
        VkImageTiling         tiling{ VK_IMAGE_TILING_OPTIMAL };
        VkImageUsageFlags     usage{ 0 };
        VkSampleCountFlagBits samples{ VK_SAMPLE_COUNT_1_BIT };
        VkMemoryPropertyFlags properties{ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT };
        VkImageAspectFlags    aspectFlags{ VK_IMAGE_ASPECT_COLOR_BIT };

    };

    class Image {
    public:
        // load LDR image from file, full upload pipeline
        static Sharedref<Image> createImageFromFile(
            const Sharedref<Device>& device,
            const Sharedref<CommandPool>& commandPool,
            const std::string& filePath,
            VkFormat format = VK_FORMAT_R8G8B8A8_SRGB,
            bool flipVertically = false);

        


        Image(const Sharedref<Device>& device, const ImageDescription& desc);
        ~Image();

        void fillImageData(size_t size, const void* pData,
            const Sharedref<CommandPool>& commandPool);

        void transitionLayout(VkImageLayout newLayout,
            VkPipelineStageFlags srcStage,
            VkPipelineStageFlags dstStage,
            const Sharedref<CommandPool>& pool);

        [[nodiscard]] VkImageView   getImageView() const { return mImageView; }
        [[nodiscard]] VkImage       getImage()     const { return mImage; }
        [[nodiscard]] VkFormat      getFormat()    const { return mDesc.format; }
        [[nodiscard]] VkImageLayout getLayout()    const { return mCurrentLayout; }
        [[nodiscard]] const ImageDescription& getDesc() const { return mDesc; }

    private:
        void createResources();
        void createImageView();
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        VkAccessFlags getSrcAccessMask(VkImageLayout layout);
        VkAccessFlags getDstAccessMask(VkImageLayout layout);

        Sharedref<Device>      mDevice;
        ImageDescription mDesc;

        VkImage        mImage{ VK_NULL_HANDLE };
        VkDeviceMemory mMemory{ VK_NULL_HANDLE };
        VkImageView    mImageView{ VK_NULL_HANDLE };
        VkImageLayout  mCurrentLayout{ VK_IMAGE_LAYOUT_UNDEFINED };
        VkExtent3D     mExtent{};
    };

} // namespace VKFW::vulkancore