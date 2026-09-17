#include "DescriptorManager.h"

namespace VKFW::renderer {

    void UniformManager::init(
        const Sharedref<vulkancore::Device>& device, int frameCount)
    {
        mDevice = device;
        mFrameCount = frameCount;
    }

    void UniformManager::attachCameraUBO() 
    {
        auto p = MakeSharedRef<vulkancore::DescriptorParameter>();
        p->mBinding = (uint32_t)mParams.size();
        p->mDescriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        p->mCount = 1;
        p->mStageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        p->mSize = sizeof(CameraUBO);

        for (int i = 0; i < mFrameCount; i++)
            p->mBuffers.push_back(
                vulkancore::DataBuffer::createUniformBuffer(mDevice, p->mSize));

        mParams.push_back(p);
    }

    void UniformManager::attachObjectUBO()
    {
        auto p = MakeSharedRef<vulkancore::DescriptorParameter>();
        p->mBinding = (uint32_t)mParams.size();
        p->mDescriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        p->mCount = 1;
        p->mStageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        p->mSize = sizeof(ObjectUBO);

        for (int i = 0; i < mFrameCount; i++)
            p->mBuffers.push_back(
                vulkancore::DataBuffer::createUniformBuffer(mDevice, p->mSize));

        mParams.push_back(p);
    }

    void UniformManager::attachTexture(const Sharedref<vulkancore::Texture>& texture)
    {
        auto p = MakeSharedRef<vulkancore::DescriptorParameter>();
        p->mBinding = (uint32_t)mParams.size();
        p->mDescriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        p->mCount = 1;
        p->mStageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        // same texture duplicated per frame (static textures)
        for (int i = 0; i < mFrameCount; i++)
            p->mTextures.push_back(texture);

        mParams.push_back(p);
    }

    void UniformManager::build() 
    {
        mLayout = MakeSharedRef<vulkancore::DescriptorSetLayout>(mDevice);
        mLayout->build(mParams);

        mPool = MakeSharedRef<vulkancore::DescriptorPool>(mDevice);
        mPool->build(mParams, mFrameCount);

        mDescriptorSet = MakeSharedRef<vulkancore::DescriptorSet>(
            mDevice, mParams, mLayout, mPool, mFrameCount);
    }

    void UniformManager::updateCameraUBO(const CameraUBO& data, int frameIndex) {
        mParams[0]->mBuffers[frameIndex]->updateBufferByMap(&data, sizeof(CameraUBO));
    }

    void UniformManager::updateObjectUBO(const ObjectUBO& data, int frameIndex)
    {
        mParams[1]->mBuffers[frameIndex]->updateBufferByMap(&data, sizeof(ObjectUBO));
    }

}