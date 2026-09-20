#pragma once
#include "render_common.h"
#include "../vulkancore/DataBuffer.h"
#include "../vulkancore/CommandBuffer.h"
#include "../vulkancore/Device.h"
#include <string>
#include <vector>

namespace VKFW::renderer {

    struct StaticMeshVertexData 
    {
        glm::vec3 mPosition;
        glm::vec3 mColor;
        glm::vec2 mUV;
    };

    class Model 
    {
    public:
        Model() = default;
        ~Model() = default;

        void loadModel(const std::string& path,
            const Sharedref<vulkancore::Device>& device);

        void draw(const Sharedref<vulkancore::CommandBuffer>& cmd);

        std::vector<VkVertexInputBindingDescription>   getBindingDescriptions()   const;
        std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() const;

        [[nodiscard]] size_t getIndexCount() const { return mIndexData.size(); }

    private:
        std::vector<StaticMeshVertexData> mVertexData{};
        std::vector<uint32_t>             mIndexData{};

        Sharedref<vulkancore::DataBuffer> mVertexBuffer{ nullptr };
        Sharedref<vulkancore::DataBuffer> mIndexBuffer{ nullptr };
    };

} // namespace VKFW::renderer