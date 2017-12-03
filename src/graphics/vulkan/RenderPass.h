#pragma once

#include <vulkan/vulkan.hpp>

class RenderPass {
public:
	RenderPass(vk::Format colorFormat, vk::Format depthFormat);
	RenderPass(RenderPass&& old) noexcept : _renderPass(old._renderPass) { old._renderPass = nullptr; }
	~RenderPass();

	operator vk::RenderPass() const { return _renderPass; }

private:
	vk::RenderPass _renderPass;
};
