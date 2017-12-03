#pragma once

#include <vulkan/vulkan.hpp>
#include "Buffer.h"

class Shader;

class Pipeline {
public:
	Pipeline(vk::Extent2D extent,
			 vk::ArrayProxy<VertexBuffer> vertexBuffers,
			 vk::ArrayProxy<vk::PushConstantRange> pushRanges,
			 const vk::DescriptorSetLayout& descSetLayout,
			 vk::ArrayProxy<Shader> shaders,
			 vk::RenderPass rp);
	Pipeline(Pipeline&& old) noexcept : _pipeline(old._pipeline) { old._pipeline = nullptr; }

	~Pipeline();

	auto getLayout() const { return _layout; }

	operator vk::Pipeline() const { return _pipeline; }

private:
	vk::PipelineLayout _layout;

	vk::Pipeline _pipeline;
};

