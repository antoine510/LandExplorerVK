#pragma once

#include <vulkan/vulkan.hpp>
#include <unordered_map>

class DescriptorSetBinding {
public:
	DescriptorSetBinding(uint32_t binding, vk::DescriptorType type, vk::ShaderStageFlags stage, vk::DescriptorBufferInfo bufferInfo);
	DescriptorSetBinding(uint32_t binding, vk::DescriptorType type, vk::ShaderStageFlags stage, vk::DescriptorImageInfo imageInfo);
	DescriptorSetBinding(uint32_t binding, vk::DescriptorType type, vk::ShaderStageFlags stage);

	bool readyForWriting() const { return _bufferInfo || _imageInfo; }

private:
	friend class DescriptorSet;

	uint32_t _binding;
	vk::DescriptorType _type;
	vk::ShaderStageFlags _stage;
	std::unique_ptr<vk::DescriptorBufferInfo> _bufferInfo = nullptr;
	std::unique_ptr<vk::DescriptorImageInfo> _imageInfo = nullptr;
};

class DescriptorSet {
public:
	DescriptorSet(vk::DescriptorPool& pool, vk::DescriptorSetLayout& layout);

	static vk::DescriptorPool createPool(const std::unordered_map<vk::DescriptorType, uint32_t>& poolMap, uint32_t maxSets);
	static void destroyPool(vk::DescriptorPool pool);

	static vk::DescriptorSetLayout createLayout(vk::ArrayProxy<DescriptorSetBinding> bindings);
	static void destroyLayout(vk::DescriptorSetLayout layout);

	void writeBinding(const DescriptorSetBinding& binding);

	operator vk::DescriptorSet() const {
		return _set;
	}

	bool isWritten() const { return _written; }
	void erase() { _written = false; }

private:
	bool _written = false;

	vk::DescriptorPool& _pool;
	vk::DescriptorSet _set;
	vk::DescriptorSetLayout& _layout;
};
