
#include "RenderPass.h"










namespace VKFW::vulkancore {


	void SubPass::validate() const
	{
		if (mColorAttachmentReferences.empty())
		{
			throw std::runtime_error("color Attachment refrences are empty");
		}
	}

	RenderPass::RenderPass(const Sharedref<Device>& device)
		:mDevice(device)
	{

	}

	RenderPass::~RenderPass()
	{
		if(mRenderPass != VK_NULL_HANDLE)
		{
			vkDestroyRenderPass(mDevice->getDevice(), mRenderPass, nullptr);
		}
	}

	void RenderPass::addAttachment(const VkAttachmentDescription& desc)
	{
		mAttachmentDescriptions.push_back(desc);
	}

	void RenderPass::addSubpass(const SubPass& subpass)
	{
		mSubPasses.push_back(subpass);
	}

	void RenderPass::addDependency(const VkSubpassDependency& dep)
	{
		mDependencies.push_back(dep);
	}

	void RenderPass::buildRenderPass()
	{
		if (mSubPasses.empty() || mAttachmentDescriptions.empty())
		{
			throw std::runtime_error("Unable to Build render pass, mSubPasses.empty() || mAttachmentDescriptions.empty()");
		}
		for (auto& sp : mSubPasses)
		{
			sp.validate();
		}
		std::vector<VkSubpassDescription> subpassDescs(mSubPasses.size());
		for (int i = 0; i < mSubPasses.size(); i++)
		{
			VkSubpassDescription& desc = subpassDescs[i];
			const auto& sp = mSubPasses[i];
			desc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			desc.colorAttachmentCount = static_cast<uint32_t>(sp.mColorAttachmentReferences.size());
			desc.pColorAttachments = sp.mColorAttachmentReferences.empty() ? nullptr : sp.mColorAttachmentReferences.data();
			desc.inputAttachmentCount = static_cast<uint32_t>(sp.mInputAttachmentReferences.size());
			desc.pInputAttachments = sp.mInputAttachmentReferences.empty() ? nullptr : sp.mInputAttachmentReferences.data();
			desc.pDepthStencilAttachment = sp.mDepthStencilAttachmentReference ? &(*sp.mDepthStencilAttachmentReference) : nullptr;
			desc.pResolveAttachments = sp.mResolveAttachmentReference ? &(*sp.mResolveAttachmentReference) : nullptr;
		}
		
		VkRenderPassCreateInfo ci{};
		ci.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		ci.attachmentCount = static_cast<uint32_t>(mAttachmentDescriptions.size());;
		ci.pAttachments = mAttachmentDescriptions.data();
		ci.dependencyCount = static_cast<uint32_t>(mDependencies.size());
		ci.pDependencies = mDependencies.data();
		ci.subpassCount = static_cast<uint32_t>(subpassDescs.size());;
		ci.pSubpasses = subpassDescs.data();
		if (vkCreateRenderPass(mDevice->getDevice(), &ci, nullptr, &mRenderPass) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create render pass");
		}
	}

}
