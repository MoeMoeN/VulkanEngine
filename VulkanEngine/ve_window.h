#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace ve {

class VeWindow {

public:
	VeWindow(uint32_t w, uint32_t h, std::string name);
	~VeWindow();

	VeWindow(const VeWindow&) = delete;
	VeWindow &operator=(const VeWindow &) = delete;

	bool shouldClose() { return glfwWindowShouldClose(window); }
	VkExtent2D getExtent() { return{ width, height }; } //need to be uint32_t, if width and height would be int then > static_cast<uint32_t> would be needed.
	bool wasWindowResized() { return framebufferResized; }
	void resetWindowResizedFlag() { framebufferResized = false; }
	GLFWwindow* getGLFWwindow() const { return window; }

	void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
private:
	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

	void initWindow();

	//window properties
	uint32_t width;
	uint32_t height;
	bool framebufferResized = false;

	std::string windowName;

	GLFWwindow* window;
};
}