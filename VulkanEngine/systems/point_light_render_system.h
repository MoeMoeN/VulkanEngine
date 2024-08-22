#pragma once

#include "../ve_pipeline.h"
#include "../ve_device.h"
#include "../ve_game_object.h"
#include "../ve_camera.h"
#include "../ve_frame_info.h"

#include <memory>
#include <vector>

namespace ve {
	class PointLightRenderSystem {

	public:
		const std::string VERT_FILEPATH = "shaders/spv/point_light.vert.spv";
		const std::string FRAG_FILEPATH = "shaders/spv/point_light.frag.spv";

		PointLightRenderSystem(VeDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~PointLightRenderSystem();

		PointLightRenderSystem(const PointLightRenderSystem&) = delete;
		PointLightRenderSystem& operator=(const PointLightRenderSystem&) = delete;

		void update(FrameInfo& frameInfo, GlobalUbo& ubo);
		void render(FrameInfo& frameInfo);

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		VeDevice& veDevice;

		std::unique_ptr<VePipeline> vePipeline;
		VkPipelineLayout pipelineLayout;
	};

}