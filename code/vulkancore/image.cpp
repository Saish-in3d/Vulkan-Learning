#include "Image.h"
#include <cassert>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace VKFW::vulkancore {

    Sharedref<Image> Image::createImageFromFile(
        const Sharedref<Device>& device,
        const Sharedref<CommandPool>& commandPool,
        const std::string& filePath,
        VkFormat format,
        bool flipVertically)
    {
        int w = 0, h = 0, channels = 0;
        stbi_set_flip_vertically_on_load(flipVertically ? 1 : 0);

        stbi_uc* pixels = stbi_load(filePath.c_str(), &w, &h, &channels, STBI_rgb_alpha);
        if (!pixels || w <= 0 || h <= 0)
            throw std::runtime_error("failed to load image: " + filePath);

        VkDeviceSize size = (VkDeviceSize)w * h * 4; // RGBA8

        ImageDescription desc{};
        desc.width = (uint32_t)w;
        desc.height = (uint32_t)h;
        desc.format = format;
        desc.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT |
            VK_IMAGE_USAGE_SAMPLED_BIT;
        desc.aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;

        auto img = MakeSharedRef<Image>(device, desc);

        // UNDEFINED -> TRANSFER_DST
        img->transitionLayout(
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            commandPool);

        // upload pixel data via staging
        img->fillImageData((size_t)size, pixels, commandPool);

        // TRANSFER_DST -> SHADER_READ_ONLY
        img->transitionLayout(
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            commandPool);

        stbi_image_free(pixels);
        return img;
    }


    Image::Image(const Sharedref<Device>& device, const ImageDescription& desc)
        : mDevice(device), mDesc(desc)
    {
        createResources();
        createImageView();
    }

    Image::~Image() {
        if (mImageView != VK_NULL_HANDLE)
            vkDestroyImageView(mDevice->getDevice(), mImageView, nullptr);
        if (mImage != VK_NULL_HANDLE)
            vkDestroyImage(mDevice->getDevice(), mImage, nullptr);
        if (mMemory != VK_NULL_HANDLE)
            vkFreeMemory(mDevice->getDevice(), mMemory, nullptr);
    }

    void Image::createResources() {
        mExtent = { mDesc.width, mDesc.height, mDesc.depth };

        VkImageCreateInfo ci{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
        ci.imageType = mDesc.imageType;
        ci.extent = { mDesc.width, mDesc.height, mDesc.depth };
        ci.mipLevels = mDesc.mipLevels;
        ci.arrayLayers = mDesc.arrayLayers;
        ci.format = mDesc.format;
        ci.tiling = mDesc.tiling;
        ci.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        ci.usage = mDesc.usage;
        ci.samples = mDesc.samples;
        ci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;


        if (vkCreateImage(mDevice->getDevice(), &ci, nullptr, &mImage) != VK_SUCCESS)
            throw std::runtime_error("failed to create image");

        VkMemoryRequirements memReq{};
        vkGetImageMemoryRequirements(mDevice->getDevice(), mImage, &memReq);

        VkMemoryAllocateInfo allocInfo{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
        allocInfo.allocationSize = memReq.size;
        allocInfo.memoryTypeIndex = findMemoryType(
            memReq.memoryTypeBits, mDesc.properties);

        if (vkAllocateMemory(mDevice->getDevice(), &allocInfo, nullptr, &mMemory) != VK_SUCCESS)
            throw std::runtime_error("failed to allocate image memory");

        vkBindImageMemory(mDevice->getDevice(), mImage, mMemory, 0);
    }

    void Image::createImageView() {
        VkImageViewCreateInfo ci{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
        ci.image = mImage;
        ci.viewType = VK_IMAGE_VIEW_TYPE_2D;
        
        ci.format = mDesc.format;
        ci.components = {
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY
        };
        ci.subresourceRange.aspectMask = mDesc.aspectFlags;
        ci.subresourceRange.baseMipLevel = 0;
        ci.subresourceRange.levelCount = mDesc.mipLevels;
        ci.subresourceRange.baseArrayLayer = 0;
        ci.subresourceRange.layerCount = mDesc.arrayLayers;
        

        if (vkCreateImageView(mDevice->getDevice(), &ci, nullptr, &mImageView) != VK_SUCCESS)
            throw std::runtime_error("failed to create image view");
    }

    void Image::fillImageData(
        size_t size, const void* pData,
        const Sharedref<CommandPool>& commandPool)
    {
        assert(pData && size > 0);

        auto staging = DataBuffer::createStageBuffer(mDevice, size, (void*)pData);

        auto cmd = MakeSharedRef<CommandBuffer>(mDevice, commandPool);
        cmd->beginCommandBuffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
        cmd->copyBufferToImage(
            staging->getBuffer(), mImage, mCurrentLayout,
            mExtent.width, mExtent.height);
        cmd->endCommandBuffer();
        cmd->submitCommandBuffer(mDevice->getGraphicQueue());
        cmd->waitCommandBuffer(mDevice->getGraphicQueue()); // wait before staging destroyed
    }

    void Image::transitionLayout(
        VkImageLayout newLayout,
        VkPipelineStageFlags srcStage,
        VkPipelineStageFlags dstStage,
        const Sharedref<CommandPool>& pool)
    {
        VkImageMemoryBarrier barrier{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
        barrier.oldLayout = mCurrentLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = mImage;
        barrier.subresourceRange.aspectMask = mDesc.aspectFlags;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = mDesc.mipLevels;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = mDesc.arrayLayers;
        barrier.srcAccessMask = getSrcAccessMask(mCurrentLayout);
        barrier.dstAccessMask = getDstAccessMask(newLayout);

        auto cmd = MakeSharedRef<CommandBuffer>(mDevice, pool);
        cmd->beginCommandBuffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
        vkCmdPipelineBarrier(
            cmd->getCommandBuffer(),
            srcStage, dstStage, 0,
            0, nullptr, 0, nullptr,
            1, &barrier);
        cmd->endCommandBuffer();
        cmd->submitCommandBuffer(mDevice->getGraphicQueue());
        vkQueueWaitIdle(mDevice->getGraphicQueue());

        mCurrentLayout = newLayout;
    }

    VkAccessFlags Image::getSrcAccessMask(VkImageLayout layout) {
        switch (layout) {
        case VK_IMAGE_LAYOUT_UNDEFINED:                        return 0;
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:             return VK_ACCESS_TRANSFER_WRITE_BIT;
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:             return VK_ACCESS_TRANSFER_READ_BIT;
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:         return VK_ACCESS_SHADER_READ_BIT;
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:         return VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL: return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        default:                                                return 0;
        }
    }

    VkAccessFlags Image::getDstAccessMask(VkImageLayout layout) {
        switch (layout) {
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:             return VK_ACCESS_TRANSFER_WRITE_BIT;
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:             return VK_ACCESS_TRANSFER_READ_BIT;
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:         return VK_ACCESS_SHADER_READ_BIT;
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:         return VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL: return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        default:                                                return 0;
        }
    }

    uint32_t Image::findMemoryType(
        uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memProp{};
        vkGetPhysicalDeviceMemoryProperties(
            mDevice->getPhysicalDevice(), &memProp);
        for (uint32_t i = 0; i < memProp.memoryTypeCount; i++) {
            if ((typeFilter & (1u << i)) &&
                (memProp.memoryTypes[i].propertyFlags & properties) == properties)
                return i;
        }
        throw std::runtime_error("failed to find suitable memory type");
    }

} // namespace VKFW::vulkancore