#pragma once
#include "vk_common.h"
#include "Image.h"
#include "Sampler.h"
#include "Device.h"
#include "CommandPool.h"
#include <string>

namespace VKFW::vulkancore {

    class Texture {
    public:
        // load from file -- creates Image + Sampler internally
        static Sharedref<Texture> create(
            const Sharedref<Device>& device,
            const Sharedref<CommandPool>& commandPool,
            const std::string& filePath);

        // wrap an existing Image with a Sampler
        static Sharedref<Texture> createFromImage(
            const Sharedref<Device>& device,
            const Sharedref<Image>& image,
            const Sharedref<Sampler>& sampler = nullptr);


        Texture(const Sharedref<Device>& device,
            const Sharedref<CommandPool>& commandPool,
            const std::string& filePath);

        Texture(const Sharedref<Device>& device,
            const Sharedref<Image>& image,
            const Sharedref<Sampler>& sampler = nullptr);

        ~Texture() = default;

        [[nodiscard]] const VkDescriptorImageInfo& getImageInfo() const { return mImageInfo; }
        [[nodiscard]] const Sharedref<Image>& getImage()    const { return mImage; }
        [[nodiscard]] const Sharedref<Sampler>& getSampler()  const { return mSampler; }

    private:
        Sharedref<Image>   mImage{ nullptr };
        Sharedref<Sampler> mSampler{ nullptr };
        Sharedref<Device>  mDevice{ nullptr };

        VkDescriptorImageInfo mImageInfo{};
    };

}