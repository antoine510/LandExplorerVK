#include "DescriptorSet.h"
#include <unordered_map>
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

DescriptorSet::DescriptorSet(vk::ArrayProxy<DescriptorSetBinding> bindings) {
	std::vector<vk::DescriptorSetLayoutBinding> layoutBindings;
	std::unordered_map<vk::DescriptorType, uint32_t> poolMap;
	for(const auto& binding : bindings) {
		layoutBindings.push_back(vk::DescriptorSetLayoutBinding(binding._binding, binding._type, 1, binding._stage));
		poolMap[binding._type]++;
	}

	vk::DescriptorSetLayoutCreateInfo descSetLayoutCI(vk::DescriptorSetLayoutCreateFlags(), (uint32_t)layoutBindings.size(), layoutBindings.data());
	_layout = VulkanState::device.createDescriptorSetLayout(descSetLayoutCI);

	std::vector<vk::DescriptorPoolSize> descPoolSizes;
	for(const auto& pair : poolMap) {
		descPoolSizes.emplace_back(pair.first, pair.second);
	}

	vk::DescriptorPoolCreateInfo descPoolCI(vk::DescriptorPoolCreateFlags(), 1, (uint32_t)descPoolSizes.size(), descPoolSizes.data());
	_pool = VulkanState::device.createDescriptorPool(descPoolCI);

	vk::DescriptorSetAllocateInfo descSetAI(_pool, 1, &_layout);
	_set = VulkanState::device.allocateDescriptorSets(descSetAI)[0];

	std::vector<vk::WriteDescriptorSet> writeDescSets;
	for(const auto& binding : bindings) {
		writeDescSets.emplace_back(_set, binding._binding, 0, 1, binding._type, binding._imageInfo.get(), binding._bufferInfo.get());
	}

	VulkanState::device.updateDescriptorSets(writeDescSets, nullptr);
}

DescriptorSet::~DescriptorSet() {
	VulkanState::device.destroyDescriptorSetLayout(_layout);
	VulkanState::device.destroyDescriptorPool(_pool);
}

void DescriptorSet::writeBinding(const DescriptorSetBinding& binding) {
	VulkanState::device.updateDescriptorSets(vk::WriteDescriptorSet(_set, binding._binding, 0, 1, binding._type, binding._imageInfo.get(), binding._bufferInfo.get()), nullptr);
}

void DescriptorSet::bind(vk::CommandBuffer& cmdBuf) {

}

