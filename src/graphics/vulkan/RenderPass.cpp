#include "RenderPass.h"
#include "VulkanCommon.h"

RenderPass::RenderPass(vk::Format colorFormat, vk::Format depthFormat) {
	std::array<vk::AttachmentDescription, 2> attachements;
	attachements[0].setFormat(colorFormat)
		.setLoadOp(vk::AttachmentLoadOp::eClear)	//TODO: No clearing
		.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
		.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
		.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

	attachements[1].setFormat(depthFormat)
		.setLoadOp(vk::AttachmentLoadOp::eClear)
		.setStoreOp(vk::AttachmentStoreOp::eDontCare)
		.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
		.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
		.setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

	vk::AttachmentReference colorAttRef(0, vk::ImageLayout::eColorAttachmentOptimal), depthAttRef(1, vk::ImageLayout::eDepthStencilAttachmentOptimal);

	auto subpass = vk::SubpassDescription()
		.setColorAttachmentCount(1)
		.setPColorAttachments(&colorAttRef)
		.setPDepthStencilAttachment(&depthAttRef);

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
