#include "RenderPass.h"
#include "VulkanCommon.h"

RenderPass::RenderPass(vk::Format colorFormat, vk::Format depthFormat) {
	std::vector<vk::AttachmentDescription> attachements(2);
	attachements[0] = vk::AttachmentDescription()
		.setFormat(colorFormat)
		.setLoadOp(vk::AttachmentLoadOp::eClear)	//TODO: No clearing
		.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
		.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
		.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

	attachements[1] = vk::AttachmentDescription()
		.setFormat(depthFormat)
		.setLoadOp(vk::AttachmentLoadOp::eClear)
		.setStoreOp(vk::AttachmentStoreOp::eDontCare)
		.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
		.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
		.setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

	auto subpass = vk::SubpassDescription()
		.setColorAttachmentCount(1)
		.setPColorAttachments(&vk::AttachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal))
		.setPDepthStencilAttachment(&vk::AttachmentReference(1, vk::ImageLayout::eDepthStencilAttachmentOptimal));

	auto subpassDependency = vk::SubpassDependency()
		.setSrcSubpass(VK_SUBPASS_EXTERNAL)
		.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
		.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
		.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite);

	vk::RenderPassCreateInfo renderPassCI(vk::RenderPassCreateFlags(), 2, attachements.data(), 1, &subpass, 1, &subpassDependency);
	_renderPass = VulkanState::device.createRenderPass(renderPassCI);
}

RenderPass::~RenderPass() {
	VulkanState::device.destroyRenderPass(_renderPass);
}
