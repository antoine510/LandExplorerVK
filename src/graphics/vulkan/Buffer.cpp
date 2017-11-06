#include "Buffer.h"
#include "VulkanCommon.h"
#include "CommandBuffer.h"

uint32_t getMemoryType(uint32_t typeBits, vk::MemoryPropertyFlags requirements_mask) {
	static auto gpuMemProps = VulkanState::gpu.getMemoryProperties();
	for(uint32_t i = 0; i < gpuMemProps.memoryTypeCount; i++) {
		if((typeBits & 1) == 1) {
			if((gpuMemProps.memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask) {
				return i;
			}
		}
		typeBits >>= 1;
	}
	SDL_assert_always(false);
	return -1;
}

AllocatedBuffer::AllocatedBuffer(vk::DeviceSize bufSize, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties)
	: _size(bufSize) {
	vk::BufferCreateInfo bufferCI(vk::BufferCreateFlags(), _size, usage);
	_buffer = VulkanState::device.createBuffer(bufferCI);

	auto bufferMemReqs = VulkanState::device.getBufferMemoryRequirements(_buffer);
	auto bufferMemtype = getMemoryType(bufferMemReqs.memoryTypeBits, properties);
	vk::MemoryAllocateInfo bufferMemoryAI(bufferMemReqs.size, bufferMemtype);
	_memory = VulkanState::device.allocateMemory(bufferMemoryAI);

	VulkanState::device.bindBufferMemory(_buffer, _memory, 0);
}

AllocatedBuffer::~AllocatedBuffer() {
	VulkanState::device.destroyBuffer(_buffer);
	VulkanState::device.freeMemory(_memory);
}

void AllocatedBuffer::update(const void* data, vk::DeviceSize offset, vk::DeviceSize updateSize) {
	if(updateSize == 0) updateSize = _size;
	void* bufferMappedMemory = VulkanState::device.mapMemory(_memory, offset, updateSize);
	memcpy(bufferMappedMemory, data, updateSize);
	VulkanState::device.unmapMemory(_memory);
}


void StagedBuffer::stageBuffer() {
	OneUseCommandBuffer oucb;
	oucb.copyBuffer(*_staging, _buffer, vk::BufferCopy(0, 0, _size));
}

