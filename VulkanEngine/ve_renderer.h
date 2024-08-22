#pragma once

#include "ve_window.h"
#include "ve_device.h"
#include "ve_swapChain.h"

//std
#include <cassert>
#include <memory>
#include <vector>

namespace ve {
	class VeRenderer {

	public:
		const std::string VERT_FILEPATH = "shaders/spv/simple_shader.vert.spv";
		const std::string FRAG_FILEPATH = "shaders/spv/simple_shader.frag.spv";

		VeRenderer(VeWindow& window, VeDevice& device);
		~VeRenderer();

		VeRenderer(const VeRenderer&) = delete;
		VeRenderer& operator=(const VeRenderer&) = delete;

		VkRenderPass getSwapChainRenderPass() const { return veSwapChain->getRenderPass(); }
		float getAspectRatio() const { return veSwapChain->extentAspectRatio(); }
		bool isFrameInProgress() const { return isFrameStarted; }

		VkCommandBuffer getCurrentCommandBuffer() const { 
			assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
			return commandBuffers[currentFrameIndex];
		}

		int getFrameIndex() const {
			assert(isFrameStarted && "Cannot get frame index when frame not in progress");
			return currentFrameIndex;
		}

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);


	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();

		VeWindow& veWindow;
		VeDevice& veDevice;
		std::unique_ptr<VeSwapChain> veSwapChain;
		std::vector<VkCommandBuffer> commandBuffers;
	
		uint32_t currentImageIndex;
		int currentFrameIndex{ 0 };
		bool isFrameStarted{ false };
	};

}