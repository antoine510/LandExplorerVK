#include "Shader.h"
#include <fstream>
#include <unordered_map>
#include "VulkanCommon.h"

Shader::Shader(const std::string& name, vk::ShaderStageFlagBits stage) : _stage(stage) {
	static std::unordered_map<vk::ShaderStageFlagBits, std::string> stageNames{
		{vk::ShaderStageFlagBits::eVertex, "vert"},
		{vk::ShaderStageFlagBits::eFragment, "frag"}
	};

	std::ifstream stream("shaders/" + name + stageNames.at(stage) + ".spv", std::ios::binary);
	std::vector<char> shaderSPV((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
	stream.close();

	std::vector<vk::PipelineShaderStageCreateInfo> pipelineShaderStagesCI;
	vk::ShaderModuleCreateInfo vertShaderCI(vk::ShaderModuleCreateFlags(), shaderSPV.size(), (const uint32_t*)shaderSPV.data());

	_shader = VulkanState::device.createShaderModule(vertShaderCI);
}

Shader::~Shader() {
	VulkanState::device.destroyShaderModule(_shader);
}

