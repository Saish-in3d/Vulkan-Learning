#include "RenderPassFactory.h"

namespace VKFW::factories {

	Sharedref<vulkancore::RenderPass> VKFW::factories::RenderPassFactory::CreateSimpleRenderPass(const Sharedref<vulkancore::Device>& device, VkFormat colorFormat)
	{

		auto rp = MakeSharedRef<vulkancore::RenderPass>(device);


		VkAttachmentDescription color{};
		color.format = colorFormat;
		color.samples = VK_SAMPLE_COUNT_1_BIT;
		color.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		color.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		color.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		color.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		color.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		color.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		rp->addAttachment(color);

		VkAttachmentReference colorRef{};
		colorRef.attachment = 0;
		colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		vulkancore::SubPass subpass;
		subpass.addColorAttachmentReference(colorRef);
		rp->addSubpass(subpass);

		VkSubpassDependency dep1{};
		dep1.srcSubpass = VK_SUBPASS_EXTERNAL;
		dep1.dstSubpass = 0;
		dep1.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dep1.srcAccessMask = 0;
		dep1.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dep1.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		rp->addDependency(dep1);

		rp->buildRenderPass();


		return rp;
	}

}