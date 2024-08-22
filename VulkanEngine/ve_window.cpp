#include "ve_window.h"

#include <stdexcept>

namespace ve {

VeWindow::VeWindow(uint32_t w, uint32_t h, std::string name) : width{ w }, height{ h }, windowName{ name } {
	initWindow();
}

VeWindow::~VeWindow() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void VeWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
	if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface");
	}
}

void VeWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
	auto veWindow = reinterpret_cast<VeWindow*>(glfwGetWindowUserPointer(window));
	veWindow->framebufferResized = true;
	veWindow->width = static_cast<uint32_t>(width);
	veWindow->height = static_cast<uint32_t>(height);
}

void VeWindow::initWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

	/*
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	glfwSetCursorPosCallback(window, VeWindow::mouse_callback);
	*/
}

}