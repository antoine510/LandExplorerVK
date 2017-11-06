#pragma once

#include <vulkan/vulkan.hpp>

class DescriptorSetBinding {
public:
	DescriptorSetBinding(uint32_t binding, vk::DescriptorType type, vk::ShaderStageFlags stage, vk::DescriptorBufferInfo bufferInfo);
	DescriptorSetBinding(uint32_t binding, vk::DescriptorType type, vk::ShaderStageFlags stage, vk::DescriptorImageInfo imageInfo);
	DescriptorSetBinding(uint32_t binding, vk::DescriptorType type, vk::ShaderStageFlags stage);

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
	DescriptorSet(vk::ArrayProxy<DescriptorSetBinding> bindings);
	~DescriptorSet();

	void writeBinding(const DescriptorSetBinding& binding);

	void bind(vk::CommandBuffer& cmdBuf);

	operator vk::DescriptorSet() const {
		return _set;
	}

	auto& getLayoutRef() const { return _layout; }

private:
	vk::DescriptorPool _pool;
	vk::DescriptorSet _set;
	vk::DescriptorSetLayout _layout;
};
