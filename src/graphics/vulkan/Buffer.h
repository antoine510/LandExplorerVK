#pragma once

#include <vulkan/vulkan.hpp>
#include <SDL_assert.h>

uint32_t getMemoryType(uint32_t typeBits, vk::MemoryPropertyFlags requirements_mask);

class AllocatedBuffer {
public:
	AllocatedBuffer(vk::DeviceSize bufSize, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties);
	AllocatedBuffer(AllocatedBuffer&& old) noexcept : _buffer(old._buffer), _memory(old._memory), _size(old._size) { old._buffer = nullptr; old._memory = nullptr; }
	~AllocatedBuffer();

	operator vk::Buffer() const {
		return _buffer;
	}

	void update(const void* data, vk::DeviceSize offset = 0, vk::DeviceSize updateSize = 0);

	vk::DescriptorBufferInfo getBufferDescriptor(uint32_t offset, uint32_t size) {
		return vk::DescriptorBufferInfo(_buffer, offset, size);
	}

protected:
	vk::Buffer _buffer;
	vk::DeviceMemory _memory;
	vk::DeviceSize _size;
};

class StagedBuffer : public AllocatedBuffer {
public:
	StagedBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage)
		: AllocatedBuffer(size, vk::BufferUsageFlagBits::eTransferDst | usage, vk::MemoryPropertyFlagBits::eDeviceLocal),
		_staging(std::make_unique<AllocatedBuffer>(size, vk::BufferUsageFlagBits::eTransferSrc,
												   vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent)) {}

	AllocatedBuffer& getStagingBuffer() { SDL_assert(_staging); return *_staging; }

	void stageBuffer(vk::DeviceSize offset, vk::DeviceSize size);
	void stageBuffer() { stageBuffer(0u, _size); }

	void lock() { _staging.release(); }

private:
	std::unique_ptr<AllocatedBuffer> _staging;
};

class VertexBuffer : public StagedBuffer {
public:
	VertexBuffer(uint32_t vertexCount, uint32_t stride, vk::Format format) :
		StagedBuffer(vertexCount * stride, vk::BufferUsageFlagBits::eVertexBuffer),
		_vertexCount(vertexCount), _stride(stride), _format(format) {}

	uint32_t getStride() const { return _stride; }
	vk::Format getFormat() const { return _format; }

private:
	vk::Format _format;
	uint32_t _vertexCount, _stride;
};
