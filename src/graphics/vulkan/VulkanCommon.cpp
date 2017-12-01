#include "VulkanCommon.h"
#include <SDL_vulkan.h>
#include <iostream>
#include "Swapchain.h"

#define VKassert(vkRes) SDL_assert(vkRes == VK_SUCCESS)

VkDebugReportCallbackEXT VulkanState::debugCallback;

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCB(
	VkDebugReportFlagsEXT flags,
	VkDebugReportObjectTypeEXT objType,
	uint64_t obj,
	size_t location,
	int32_t code,
	const char* layerPrefix,
	const char* msg,
	void* userData) {

	std::cerr << "Vulkan validation: " << msg << std::endl;

	return VK_FALSE;
}

vk::Instance VulkanState::inst;
vk::PhysicalDevice VulkanState::gpu;
vk::Device VulkanState::device;
vk::Queue VulkanState::deviceQueue;
vk::SurfaceKHR VulkanState::windowKHR;
vk::CommandPool VulkanState::cmdPool;
vk::CommandPool VulkanState::cmdPoolTransient;

void VulkanState::setup(SDL_Window* window) {

	unsigned int extCount;
	SDL_Vulkan_GetInstanceExtensions(window, &extCount, NULL);

	std::vector<const char*> extensions(extCount), layers;

	SDL_Vulkan_GetInstanceExtensions(window, &extCount, extensions.data());
#ifdef _DEBUG
	extensions.push_back("VK_EXT_debug_report");
	layers.push_back("VK_LAYER_LUNARG_standard_validation");
#endif // DEBUG

	vk::ApplicationInfo appInfo("VulkanTest", 1, "CustomEngine", 1, VK_API_VERSION_1_0);
	vk::InstanceCreateInfo instCreateInfo(vk::InstanceCreateFlags(), &appInfo,
		(uint32_t)layers.size(), layers.data(),
										  (uint32_t)extensions.size(), extensions.data());
	inst = vk::createInstance(instCreateInfo);

#ifdef _DEBUG
	vk::DebugReportCallbackCreateInfoEXT debugInfo(vk::DebugReportFlagBitsEXT::eError | vk::DebugReportFlagBitsEXT::eWarning, debugCB);
	auto vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)inst.getProcAddr("vkCreateDebugReportCallbackEXT");
	VKassert(vkCreateDebugReportCallbackEXT(inst, &(VkDebugReportCallbackCreateInfoEXT)debugInfo, nullptr, &debugCallback));
#endif // DEBUG

	VkSurfaceKHR cWindowKHR = nullptr;
	SDL_assert_always(SDL_Vulkan_CreateSurface(window, inst, &cWindowKHR));
	windowKHR = vk::SurfaceKHR(cWindowKHR);

	gpu = inst.enumeratePhysicalDevices()[0];

	auto queueFamilyProps = gpu.getQueueFamilyProperties();
	gpu.getSurfaceSupportKHR(0, windowKHR);

	uint32_t selectedQueueFamily = -1;
	auto queueFamilies = gpu.getQueueFamilyProperties();
	for(auto i = 0; i < queueFamilies.size(); ++i) {
		if(queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics && gpu.getSurfaceSupportKHR(i, windowKHR)) {
			selectedQueueFamily = i;
			break;
		}
	}
	SDL_assert(selectedQueueFamily != -1);

	std::vector<float> queuePriorities = { 0.0f };
	vk::DeviceQueueCreateInfo queueCreateInfo(vk::DeviceQueueCreateFlags(), selectedQueueFamily, 1, queuePriorities.data());

	std::vector<const char*> deviceExtensions;
	deviceExtensions.push_back("VK_KHR_swapchain");

	//auto gpuFeatures = vk::PhysicalDeviceFeatures().setSamplerAnisotropy(true);

	auto deviceCreateInfo = vk::DeviceCreateInfo()
		.setQueueCreateInfoCount(1)
		.setPQueueCreateInfos(&queueCreateInfo)
		.setEnabledExtensionCount((uint32_t)deviceExtensions.size())
		.setPpEnabledExtensionNames(deviceExtensions.data());
	device = gpu.createDevice(deviceCreateInfo);
	deviceQueue = device.getQueue(selectedQueueFamily, 0);

	vk::CommandPoolCreateInfo cmdPoolCI(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, selectedQueueFamily);
	cmdPool = device.createCommandPool(cmdPoolCI);
	cmdPoolCI.setFlags(vk::CommandPoolCreateFlagBits::eTransient);
	cmdPoolTransient = device.createCommandPool(cmdPoolCI);
}

void VulkanState::teardown() {
	device.destroyCommandPool(cmdPool);
	device.destroyCommandPool(cmdPoolTransient);
	device.destroy();

#ifdef _DEBUG
	auto vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)inst.getProcAddr("vkDestroyDebugReportCallbackEXT");
	vkDestroyDebugReportCallbackEXT(inst, debugCallback, nullptr);
#endif // DEBUG

	inst.destroy();
}
