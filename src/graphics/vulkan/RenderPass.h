#pragma once

#include <vulkan/vulkan.hpp>

class RenderPass {
public:
	RenderPass(vk::Format colorFormat, vk::Format depthFormat);
	~RenderPass();

	operator vk::RenderPass() const { return _renderPass; }

private:
	vk::RenderPass _renderPass;
};
