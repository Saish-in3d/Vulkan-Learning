#include "Texture.h"

namespace VKFW::vulkancore {

    Sharedref<Texture> Texture::create(
        const Sharedref<Device>& device,
        const Sharedref<CommandPool>& commandPool,
        const std::string& filePath)
    {
        return MakeSharedRef<Texture>(device, commandPool, filePath);
    }

    Sharedref<Texture> Texture::createFromImage(
        const Sharedref<Device>& device,
        const Sharedref<Image>& image,
        const Sharedref<Sampler>& sampler)
    {
        return MakeSharedRef<Texture>(device, image, sampler);
    }

    Texture::Texture(
        const Sharedref<Device>& device,
        const Sharedref<CommandPool>& commandPool,
        const std::string& filePath)
        : mDevice(device)
    {
        mImage = Image::createImageFromFile(device, commandPool, filePath);
        mSampler = MakeSharedRef<Sampler>(device);

        mImageInfo.imageLayout = mImage->getLayout();
        mImageInfo.imageView = mImage->getImageView();
        mImageInfo.sampler = mSampler->getSampler();
    }

    Texture::Texture(
        const Sharedref<Device>& device,
        const Sharedref<Image>& image,
        const Sharedref<Sampler>& sampler)
        : mDevice(device), mImage(image)
    {
        if (!mImage)
            throw std::runtime_error("Texture: image must not be null");

        mSampler = sampler ? sampler : MakeSharedRef<Sampler>(device);

        mImageInfo.imageLayout = mImage->getLayout();
        mImageInfo.imageView = mImage->getImageView();
        mImageInfo.sampler = mSampler->getSampler();
    }

}