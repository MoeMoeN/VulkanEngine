#pragma once

#include "../ve_pipeline.h"
#include "../ve_device.h"
#include "../ve_game_object.h"
#include "../ve_camera.h"
#include "../ve_frame_info.h"

#include <memory>
#include <vector>

namespace ve {
	class GridRenderSystem {

	public:
		const std::string VERT_FILEPATH = "shaders/spv/global_grid.vert.spv";
		const std::string FRAG_FILEPATH = "shaders/spv/global_grid.frag.spv";

		GridRenderSystem(VeDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~GridRenderSystem();

		GridRenderSystem(const GridRenderSystem&) = delete;
		GridRenderSystem& operator=(const GridRenderSystem&) = delete;

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