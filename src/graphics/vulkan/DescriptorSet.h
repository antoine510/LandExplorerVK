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
	~DescriptorSet() {}

	static vk::DescriptorPool createPool(const std::unordered_map<vk::DescriptorType, uint32_t>& poolMap, uint32_t maxSets);
	static void destroyPool(vk::DescriptorPool pool);

	static vk::DescriptorSetLayout createLayout(vk::ArrayProxy<DescriptorSetBinding> bindings);
	static void destroyLayout(vk::DescriptorSetLayout layout);

	void writeBinding(const DescriptorSetBinding& binding);

	operator vk::DescriptorSet() const { return _set; }

	bool isWritten() const { return _written; }
	void erase() { _written = false; }

private:
	bool _written = false;

	vk::DescriptorPool& _pool;
	vk::DescriptorSet _set;
	vk::DescriptorSetLayout& _layout;
};

class DescriptorSetRef {
public:
	DescriptorSetRef(DescriptorSet* ref, vk::ArrayProxy<DescriptorSetBinding> bindings) : _ref(ref) {
		for(const auto& binding : bindings) ref->writeBinding(binding);
	}

	DescriptorSetRef(DescriptorSetRef&& old) noexcept : _ref(old._ref) { old._ref = nullptr; }
	~DescriptorSetRef() { if(_ref != nullptr) _ref->erase(); }

	operator DescriptorSet() const { return *_ref; }
	operator vk::DescriptorSet() const { return _ref->operator vk::DescriptorSet(); }

private:
	DescriptorSet* _ref;
};
