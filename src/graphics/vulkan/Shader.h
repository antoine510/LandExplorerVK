#pragma once

#include <vulkan/vulkan.hpp>

class Shader {
public:
	Shader(const std::string& name, vk::ShaderStageFlagBits stage);
	Shader(Shader&& old) : _shader(old._shader), _stage(old._stage) { old._shader = nullptr; }
	~Shader();

	operator vk::ShaderModule() const {
		return _shader;
	}

	auto getPipelineStageInfo() const {
		return vk::PipelineShaderStageCreateInfo(vk::PipelineShaderStageCreateFlags(), _stage, _shader, "main");
	}

private:
	vk::ShaderModule _shader;
	vk::ShaderStageFlagBits _stage;
};

