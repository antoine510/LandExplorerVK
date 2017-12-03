#include "DescriptorSet.h"
#include "VulkanCommon.h"

DescriptorSetBinding::DescriptorSetBinding(uint32_t binding, vk::DescriptorType type, vk::ShaderStageFlags stage, vk::DescriptorBufferInfo bufferInfo) :
	DescriptorSetBinding(binding, type, stage) {
	_bufferInfo = std::make_unique<vk::DescriptorBufferInfo>(bufferInfo);
}

DescriptorSetBinding::DescriptorSetBinding(uint32_t binding, vk::DescriptorType type, vk::ShaderStageFlags stage, vk::DescriptorImageInfo imageInfo) :
	DescriptorSetBinding(binding, type, stage) {
	_imageInfo = std::make_unique<vk::DescriptorImageInfo>(imageInfo);
}

DescriptorSetBinding::DescriptorSetBinding(uint32_t binding, vk::DescriptorType type, vk::ShaderStageFlags stage) : _binding(binding), _type(type), _stage(stage) {}

DescriptorSet::DescriptorSet(vk::DescriptorPool& pool, vk::DescriptorSetLayout& layout) : _pool(pool), _layout(layout) {
	vk::DescriptorSetAllocateInfo descSetAI(_pool, 1, &_layout);
	_set = VulkanState::device.allocateDescriptorSets(descSetAI)[0];
}

void DescriptorSet::writeBinding(const DescriptorSetBinding& binding) {
	VulkanState::device.updateDescriptorSets(vk::WriteDescriptorSet(_set, binding._binding, 0, 1, binding._type, binding._imageInfo.get(), binding._bufferInfo.get()), nullptr);
	_written = true;
}

vk::DescriptorPool DescriptorSet::createPool(const std::unordered_map<vk::DescriptorType, uint32_t>& poolMap, uint32_t maxSets) {
	std::vector<vk::DescriptorPoolSize> descPoolSizes;
	for(const auto& pair : poolMap) {
		descPoolSizes.emplace_back(pair.first, pair.second * maxSets);
	}

	vk::DescriptorPoolCreateInfo descPoolCI(vk::DescriptorPoolCreateFlags(), maxSets, (uint32_t)descPoolSizes.size(), descPoolSizes.data());
	return VulkanState::device.createDescriptorPool(descPoolCI);
}

void DescriptorSet::destroyPool(vk::DescriptorPool pool) {
	VulkanState::device.destroyDescriptorPool(pool);
}

vk::DescriptorSetLayout DescriptorSet::createLayout(vk::ArrayProxy<DescriptorSetBinding> bindings) {
	std::vector<vk::DescriptorSetLayoutBinding> layoutBindings;
	for(const auto& binding : bindings) {
		layoutBindings.push_back(vk::DescriptorSetLayoutBinding(binding._binding, binding._type, 1, binding._stage));
	}

	vk::DescriptorSetLayoutCreateInfo descSetLayoutCI(vk::DescriptorSetLayoutCreateFlags(), (uint32_t)layoutBindings.size(), layoutBindings.data());
	return VulkanState::device.createDescriptorSetLayout(descSetLayoutCI);
}

void DescriptorSet::destroyLayout(vk::DescriptorSetLayout layout) {
	VulkanState::device.destroyDescriptorSetLayout(layout);
}

