#pragma once

#include "ve_window.h"
#include "ve_device.h"
#include "ve_game_object.h"
#include "ve_renderer.h"
#include "ve_descriptors.h"

#include <memory>
#include <vector>

namespace ve {
class App {

public:
	static constexpr uint32_t WIDTH = 800;
	static constexpr uint32_t HEIGHT = 600;
	const std::string VERT_FILEPATH = "shaders/spv/simple_shader.vert.spv";
	const std::string FRAG_FILEPATH = "shaders/spv/simple_shader.frag.spv";

	App();
	~App();

	App(const App&) = delete;
	App& operator=(const App&) = delete;

	void run();

private:
	void loadGameObjects();

	/*
	 temp functions to create geometry
	*/
	std::unique_ptr<VeModel> createCubeGeometry(VeDevice& device, glm::vec3 offset);

	/*order here is important (initialization and cleaning up)*/
	VeWindow veWindow{ WIDTH, HEIGHT, "Hello Vulkan!" };
	VeDevice veDevice{ veWindow };
	VeRenderer veRenderer{ veWindow, veDevice };

	//order of decalations matters
	std::unique_ptr<VeDescriptorPool> globalPool{};
	VeGameObject::Map veGameObjects;
};

}