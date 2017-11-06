#pragma once

#include <vulkan/vulkan.hpp>
#include "VulkanCommon.h"

class OneUseCommandBuffer : public vk::CommandBuffer {
public:
	OneUseCommandBuffer() : vk::CommandBuffer(
		(VkCommandBuffer)VulkanState::device.allocateCommandBuffers(
			vk::CommandBufferAllocateInfo(VulkanState::cmdPoolTransient, vk::CommandBufferLevel::ePrimary, 1))[0]) {
		begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
	}

	~OneUseCommandBuffer() {
		end();
		VulkanState::deviceQueue.submit(vk::SubmitInfo().setCommandBufferCount(1).setPCommandBuffers(this), vk::Fence());
		VulkanState::deviceQueue.waitIdle();
		VulkanState::device.freeCommandBuffers(VulkanState::cmdPoolTransient, *this);
	}

private:
};
