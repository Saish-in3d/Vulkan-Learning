#pragma once
#include "vk_common.h"
#include "device.h"

namespace VKFW::vulkancore {

    class SubPass {
    public:
        SubPass() = default;
        ~SubPass() = default;

        void addColorAttachmentReference(const VkAttachmentReference& ref) {
            mColorAttachmentReferences.push_back(ref);
        }
        void addInputAttachmentReference(const VkAttachmentReference& ref) {
            mInputAttachmentReferences.push_back(ref);
        }
        void setDepthStencilAttachmentReference(const VkAttachmentReference& ref) {
            mDepthStencilAttachmentReference = ref;
        }
        void setResolveAttachmentReference(const VkAttachmentReference& ref) {
            mResolveAttachmentReference = ref;
        }
        void validate() const;

    private:
        friend class RenderPass;
        std::vector<VkAttachmentReference>   mColorAttachmentReferences{};
        std::vector<VkAttachmentReference>   mInputAttachmentReferences{};
        std::optional<VkAttachmentReference> mDepthStencilAttachmentReference{};
        std::optional<VkAttachmentReference> mResolveAttachmentReference{};
    };

    class RenderPass {
    public:
        explicit RenderPass(const Sharedref<Device>& device);
        ~RenderPass();

        void addAttachment(const VkAttachmentDescription& desc);
        void addSubpass(const SubPass& subpass);
        void addDependency(const VkSubpassDependency& dep);
        void buildRenderPass();

        [[nodiscard]] VkRenderPass getRenderPass() const { return mRenderPass; }

    private:
        Sharedref<Device>   mDevice;
        VkRenderPass  mRenderPass{ VK_NULL_HANDLE };

        std::vector<VkAttachmentDescription> mAttachmentDescriptions{};
        std::vector<SubPass>                 mSubPasses{};
        std::vector<VkSubpassDependency>     mDependencies{};
    };

} // namespace VKFW::vulkancore