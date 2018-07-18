#pragma once

#include <vulkan/vulkan.hpp>
#include <SDL_assert.h>

uint32_t getMemoryType(uint32_t typeBits, vk::MemoryPropertyFlags requirements_mask);

class AllocatedBuffer {
public:
	AllocatedBuffer(vk::DeviceSize bufSize, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties);
	AllocatedBuffer(AllocatedBuffer&& old) noexcept : _buffer(old._buffer), _memory(old._memory), _size(old._size) { old._buffer = nullptr; old._memory = nullptr; }
	virtual ~AllocatedBuffer();

	operator vk::Buffer() const { return _buffer; }
	vk::DeviceSize getSize() const { return _size; }

protected:
	vk::Buffer _buffer;
	vk::DeviceMemory _memory;
	vk::DeviceSize _size;
};

class HostBuffer : public AllocatedBuffer {
public:
	HostBuffer(vk::DeviceSize bufSize) : AllocatedBuffer(bufSize, vk::BufferUsageFlagBits::eTransferSrc,
														 vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent) {}
	template <typename T>
	HostBuffer(std::vector<T> data) : AllocatedBuffer(data.size() * sizeof(T), vk::BufferUsageFlagBits::eTransferSrc,
													  vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent) {
		update(data.data(), 0, _size);
	}
	HostBuffer(const void* data, vk::DeviceSize size) : AllocatedBuffer(size, vk::BufferUsageFlagBits::eTransferSrc,
																		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent) {
		update(data, 0, size);
	}
	HostBuffer(HostBuffer&& old) = default;
	virtual ~HostBuffer() = default;

	void update(const void* data, vk::DeviceSize offset, vk::DeviceSize updateSize);

	void* mapMemory(vk::DeviceSize offset = 0, vk::DeviceSize size = 0);
	void unmapMemory();
};

class DeviceBuffer : public AllocatedBuffer {
public:
	DeviceBuffer(vk::DeviceSize bufSize, vk::BufferUsageFlags usage = vk::BufferUsageFlags())
		: AllocatedBuffer(bufSize, vk::BufferUsageFlagBits::eTransferDst | usage, vk::MemoryPropertyFlagBits::eDeviceLocal) {}
	DeviceBuffer(DeviceBuffer&& old) = default;
	virtual ~DeviceBuffer() = default;

	void update(const HostBuffer& src) { update(src, 0, 0, src.getSize()); }
	void update(const HostBuffer& src, vk::DeviceSize dstOffset) { update(src, 0, dstOffset, src.getSize()); }
	void update(const HostBuffer& src, vk::DeviceSize srcOffset, vk::DeviceSize dstOffset, vk::DeviceSize updateSize);

	vk::DescriptorBufferInfo getBufferDescriptor(uint32_t offset, uint32_t size) {
		return vk::DescriptorBufferInfo(_buffer, offset, size);
	}
};

class VertexBuffer : public DeviceBuffer {
public:
	VertexBuffer(uint32_t vertexCount, uint32_t stride, vk::Format format) :
		DeviceBuffer(vertexCount * stride, vk::BufferUsageFlagBits::eVertexBuffer),
		_vertexCount(vertexCount), _stride(stride), _format(format) {}
	VertexBuffer(VertexBuffer&& old) = default;
	virtual ~VertexBuffer() = default;

	uint32_t getStride() const { return _stride; }
	vk::Format getFormat() const { return _format; }

private:
	vk::Format _format;
	uint32_t _vertexCount, _stride;
};
