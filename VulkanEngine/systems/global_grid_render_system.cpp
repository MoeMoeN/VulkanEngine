#include "global_grid_render_system.h"


// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <map>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
namespace ve {

	struct GlobalGridPushConstants {
		//glm::vec4 position{};
		//glm::vec4 color{};
		float thickness;
	};

	GridRenderSystem::GridRenderSystem(VeDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : veDevice{ device } {
		createPipelineLayout(globalSetLayout);
		createPipeline(renderPass);
	}

	GridRenderSystem::~GridRenderSystem() {
		vkDestroyPipelineLayout(veDevice.device(), pipelineLayout, nullptr);
	}

	void GridRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(GlobalGridPushConstants);

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;//&pushConstantRange;
		if (vkCreatePipelineLayout(veDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout");
		}
	}

	void GridRenderSystem::createPipeline(VkRenderPass renderPass) {
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		VePipeline::defaultPipelineConfigInfo(pipelineConfig);
		VePipeline::enableAlphaBlending(pipelineConfig);
		pipelineConfig.attributeDescriptions.clear(); //clear cause point light are special cases when we dont need attribute and binding descriptions
		pipelineConfig.bindingDescriptions.clear();
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		vePipeline = std::make_unique<VePipeline>(veDevice, pipelineConfig, VERT_FILEPATH, FRAG_FILEPATH);
	}


	void GridRenderSystem::update(FrameInfo& frameInfo, GlobalUbo& ubo) {

		auto rotateLight = glm::rotate(glm::mat4(1.f), frameInfo.deltaTime, { 0.f, -1.f, 0.f });

		int GridIndex = 0;
		for (auto& kv : frameInfo.gameObjects) {
			auto& obj = kv.second;
			if (!obj.isGlobalGrid) continue;

			assert(GridIndex < 1 && "There should be only one global grid");

			//update light position
			//obj.transform.translation = glm::vec3(rotateLight * glm::vec4(obj.transform.translation, 1.f));

			//copy light to ubo
			//ubo.pointLight[GridIndex].position = glm::vec4(obj.transform.translation, 1.f);
			//ubo.pointLight[GridIndex].color = glm::vec4(obj.color, obj.pointLight->lightIntensity);
			GridIndex += 1;
		}
		//ubo.numLights = GridIndex;
	}

	void GridRenderSystem::render(FrameInfo& frameInfo) {
		//sort lights based on distance from camera
		std::map<float, VeGameObject::id_t> sorted;
		for (auto& kv : frameInfo.gameObjects) {
			auto& obj = kv.second;
			if (!obj.isGlobalGrid) continue;

			//auto offset = frameInfo.camera.getPosition() - obj.transform.translation;
			//float distSquared = glm::dot(offset, offset);
			sorted[0] = obj.getId();
		}

		vePipeline->bind(frameInfo.commandBuffer);

		vkCmdBindDescriptorSets(
			frameInfo.commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipelineLayout,
			0,
			1,
			&frameInfo.globalDescriptorSet,
			0,
			nullptr
		);

		// iterate thour sorted lights in reverse
		for (auto it = sorted.rbegin(); it != sorted.rend(); ++it) {
			//use game obj id to find light object
			auto& obj = frameInfo.gameObjects.at(it->second);

			GlobalGridPushConstants push{};
			push.thickness = 0.05f;
			//push.position = glm::vec4(obj.transform.translation, 1.f);
			//push.color = glm::vec4(obj.color, obj.pointLight->lightIntensity);
			//push.radius = obj.transform.scale.x;

			vkCmdPushConstants(
				frameInfo.commandBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(GlobalGridPushConstants),
				&push
			);
			vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
		}

	}

}