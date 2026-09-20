#include "Model.h"
#include <stdexcept>
#include <unordered_map>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

namespace VKFW::renderer {

    void Model::loadModel(const std::string& path,
        const Sharedref<vulkancore::Device>& device)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t>    shapes;
        std::vector<tinyobj::material_t> materials;
        std::string err, warn;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials,
            &warn, &err, path.c_str()))
        {
            throw std::runtime_error("failed to load model: " + err);
        }

        mVertexData.clear();
        mIndexData.clear();

        // key: "vi/ti/ni" -- maps unique v/vt/vn combinations to vertex index
        // OBJ has 3 separate index arrays; Vulkan uses a single index
        // this map deduplicates: same combination = same vertex = same index
        std::unordered_map<std::string, uint32_t> uniqueVertices;

        for (const auto& shape : shapes) 
        {
            for (const auto& index : shape.mesh.indices) 
            {

                std::string key =
                    std::to_string(index.vertex_index) + "/" +
                    std::to_string(index.texcoord_index) + "/" +
                    std::to_string(index.normal_index);

                if (uniqueVertices.count(key) == 0) 
                {
                    StaticMeshVertexData v{};

                    v.mPosition = glm::vec3(
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]);

                    v.mColor = glm::vec3(1.0f);

                    if (index.texcoord_index >= 0) 
                    {
                        v.mUV = glm::vec2(
                            attrib.texcoords[2 * index.texcoord_index + 0],
                            1.0f - attrib.texcoords[2 * index.texcoord_index + 1]);
                        // flip V: OBJ origin is bottom-left, Vulkan is top-left
                    }
                    else 
                    {
                        v.mUV = glm::vec2(0.0f);
                    }

                    uniqueVertices[key] = (uint32_t)mVertexData.size(); //Saving indexes in unordered map for dedup
                    mVertexData.push_back(v); //actual buffer for further processing 
                } //if statement end

                mIndexData.push_back(uniqueVertices[key]); //  (combining index data for each)
            }
        }

        mVertexBuffer = vulkancore::DataBuffer::createVertexBuffer(
            device,
            mVertexData.size() * sizeof(StaticMeshVertexData),
            mVertexData.data());

        mIndexBuffer = vulkancore::DataBuffer::createIndexBuffer(
            device,
            mIndexData.size() * sizeof(uint32_t),
            mIndexData.data());
    }

    void Model::draw(const Sharedref<vulkancore::CommandBuffer>& cmd)
    {
        std::vector<VkBuffer>     buffers = { mVertexBuffer->getBuffer() };
        std::vector<VkDeviceSize> offsets = { 0 };
        cmd->bindVertexBuffer(buffers, 0, offsets);
        cmd->bindIndexBuffer(mIndexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
        cmd->drawIndexed((uint32_t)mIndexData.size(), 1, 0, 0, 0);
    }

    std::vector<VkVertexInputBindingDescription>
        Model::getBindingDescriptions() const
    {
        VkVertexInputBindingDescription b{};
        b.binding = 0;
        b.stride = sizeof(StaticMeshVertexData);
        b.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return { b };
    }

    std::vector<VkVertexInputAttributeDescription>
        Model::getAttributeDescriptions() const
    {
        std::vector<VkVertexInputAttributeDescription> attrs(3);

        attrs[0].binding = 0;
        attrs[0].location = 0;
        attrs[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attrs[0].offset = offsetof(StaticMeshVertexData, mPosition);

        attrs[1].binding = 0;
        attrs[1].location = 1;
        attrs[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attrs[1].offset = offsetof(StaticMeshVertexData, mColor);

        attrs[2].binding = 0;
        attrs[2].location = 2;
        attrs[2].format = VK_FORMAT_R32G32_SFLOAT;
        attrs[2].offset = offsetof(StaticMeshVertexData, mUV);


        return attrs;
    }

} // namespace VKFW::renderer