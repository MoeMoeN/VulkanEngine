#pragma once

#include "../ve_pipeline.h"
#include "../ve_device.h"
#include "../ve_game_object.h"
#include "../ve_camera.h"
#include "../ve_frame_info.h"

#include <memory>
#include <vector>

namespace ve {
	class SimpleRenderSystem {

	public:
		const std::string VERT_FILEPATH = "shaders/spv/simple_shader.vert.spv";
		const std::string FRAG_FILEPATH = "shaders/spv/simple_shader.frag.spv";

		SimpleRenderSystem(VeDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void renderGameObjects(FrameInfo& frameInfo);

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		VeDevice& veDevice;

		std::unique_ptr<VePipeline> vePipeline;
		VkPipelineLayout pipelineLayout;
	};

}