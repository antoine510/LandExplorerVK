#pragma once

#include <vulkan/vulkan.hpp>

class Swapchain;
struct SDL_Window;

class VulkanState {
public:

	static void setup(SDL_Window* window);
	static void teardown();

	static vk::Instance inst;
	static vk::PhysicalDevice gpu;
	static vk::Device device;
	static vk::Queue deviceQueue;
	static vk::SurfaceKHR windowKHR;

	static vk::CommandPool cmdPool;
	static vk::CommandPool cmdPoolTransient;

private:
	static VkDebugReportCallbackEXT debugCallback;
};
