#pragma once
#include "../vulkancore/DescriptorSetLayout.h"
#include "../vulkancore/DescriptorPool.h"
#include "../vulkancore/DescriptorSet.h"
#include "../vulkancore/DataBuffer.h"
#include "../vulkancore/Device.h"
#include "../vulkancore/CommandPool.h"
#include "../vulkancore/Texture.h"
#include "render_common.h"

namespace VKFW::renderer {

    class UniformManager {
    public:
        UniformManager() = default;
        ~UniformManager() = default;

        void init(const Sharedref<vulkancore::Device>& device, int frameCount);

        void attachCameraUBO();   // binding 0: view + proj
        void attachObjectUBO();   // binding 1: model matrix
        void attachTexture(const Sharedref<vulkancore::Texture>& texture);
        

        void build();

        void updateCameraUBO(const CameraUBO& data, int frameIndex);
        void updateObjectUBO(const ObjectUBO& data, int frameIndex);

        [[nodiscard]] Sharedref<vulkancore::DescriptorSetLayout> getLayout() const { return mLayout; }
        [[nodiscard]] VkDescriptorSet getDescriptorSet(int frameIndex) const 
        {
            return mDescriptorSet->getDescriptorSet(frameIndex);
        }

    private:
        Sharedref<vulkancore::Device>                        mDevice{ nullptr };
        int                                            mFrameCount{ 1 };
        std::vector<Sharedref<vulkancore::DescriptorParameter>> mParams{};

        Sharedref<vulkancore::DescriptorSetLayout> mLayout{ nullptr };
        Sharedref<vulkancore::DescriptorPool>      mPool{ nullptr };
        Sharedref<vulkancore::DescriptorSet>       mDescriptorSet{ nullptr };
    };

}