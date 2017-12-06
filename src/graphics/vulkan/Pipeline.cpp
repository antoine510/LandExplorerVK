#include "Pipeline.h"
#include "VulkanCommon.h"
#include "Shader.h"

Pipeline::Pipeline(vk::Extent2D extent,
				   vk::ArrayProxy<VertexBuffer> vertexBuffers,
				   vk::ArrayProxy<vk::PushConstantRange> pushRanges,
				   const vk::DescriptorSetLayout& descSetLayout,
				   vk::ArrayProxy<Shader> shaders,
				   vk::RenderPass rp) {
	std::vector<vk::VertexInputBindingDescription> vertexInputBinding;
	std::vector<vk::VertexInputAttributeDescription> vertexInputAttributes;
	uint32_t location = 0;
	for(const auto& vertexBuffer : vertexBuffers) {
		vertexInputBinding.emplace_back(0, vertexBuffer.getStride());
		vertexInputAttributes.emplace_back(location++, 0, vertexBuffer.getFormat(), 0);
	}

	vk::PipelineLayoutCreateInfo pipelineLayoutCI(vk::PipelineLayoutCreateFlags(), 1, &descSetLayout, pushRanges.size(), pushRanges.data());
	_layout = VulkanState::device.createPipelineLayout(pipelineLayoutCI);

	std::vector<vk::PipelineShaderStageCreateInfo> pipelineShaderStagesCI;
	for(const auto& shader : shaders) {
		pipelineShaderStagesCI.push_back(shader.getPipelineStageInfo());
	}

	//vk::PipelineDynamicStateCreateInfo pipelineDynamicCI;
	vk::PipelineVertexInputStateCreateInfo pipelineVertexInputCI(vk::PipelineVertexInputStateCreateFlags(),
		(uint32_t)vertexInputBinding.size(), vertexInputBinding.data(), (uint32_t)vertexInputAttributes.size(), vertexInputAttributes.data());
	vk::PipelineInputAssemblyStateCreateInfo pipelineInputAssemblyCI(vk::PipelineInputAssemblyStateCreateFlags(),
																	 vk::PrimitiveTopology::eTriangleList);
	auto pipelineRasterizationCI = vk::PipelineRasterizationStateCreateInfo()
		.setCullMode(vk::CullModeFlagBits::eBack)
		.setLineWidth(1.0f);

	auto pipelineColorBlendState = vk::PipelineColorBlendAttachmentState(true, vk::BlendFactor::eSrcAlpha, vk::BlendFactor::eOneMinusSrcAlpha, vk::BlendOp::eAdd,
																		 vk::BlendFactor::eSrcAlpha, vk::BlendFactor::eOneMinusSrcAlpha, vk::BlendOp::eAdd,
																		 vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
																		 vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
	auto pipelineColorBlendCI = vk::PipelineColorBlendStateCreateInfo()
		.setAttachmentCount(1)
		.setPAttachments(&pipelineColorBlendState)
		.setBlendConstants({{1, 1, 1, 1}});

	vk::Viewport pipelineViewport(0.0f, 0.0f, (float)extent.width, (float)extent.height, 0.0f, 1.0f);
	vk::Rect2D pipelineScissor(vk::Offset2D(), extent);
	vk::PipelineViewportStateCreateInfo pipelineViewportCI(vk::PipelineViewportStateCreateFlags(), 1, &pipelineViewport, 1, &pipelineScissor);
	//vk::PipelineDepthStencilStateCreateInfo pipelineDepthStencilCI(vk::PipelineDepthStencilStateCreateFlags(), true, true, vk::CompareOp::eLessOrEqual);
	vk::PipelineDepthStencilStateCreateInfo pipelineDepthStencilCI(vk::PipelineDepthStencilStateCreateFlags(), false);
	vk::PipelineMultisampleStateCreateInfo pipelineMultisampleCI;
	vk::GraphicsPipelineCreateInfo pipelineCI(vk::PipelineCreateFlags(), (uint32_t)pipelineShaderStagesCI.size(),
											  pipelineShaderStagesCI.data(),
											  &pipelineVertexInputCI,
											  &pipelineInputAssemblyCI,
											  nullptr,
											  &pipelineViewportCI,
											  &pipelineRasterizationCI,
											  &pipelineMultisampleCI,
											  &pipelineDepthStencilCI,
											  &pipelineColorBlendCI,
											  nullptr,
											  _layout,
											  rp, 0);
	_pipeline = VulkanState::device.createGraphicsPipeline(vk::PipelineCache(), pipelineCI);
}

Pipeline::~Pipeline() {
	VulkanState::device.destroyPipelineLayout(_layout);
	VulkanState::device.destroyPipeline(_pipeline);
}
