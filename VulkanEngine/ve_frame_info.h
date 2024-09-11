#pragma once

#include "ve_camera.h"
#include "ve_game_object.h"
//lib
#include <vulkan/vulkan.h>

namespace ve {

#define MAX_LIGHTS 10

	struct PointLight {
		glm::vec4 position{}; //ignore w
		glm::vec4 color{}; //w is intensity
	};

	//note: to be honest, just don;t use vec3. Always when we need to use vec3 just use vec4
	struct GlobalUbo {
		alignas(16) glm::mat4 projection{1.f};
		alignas(16) glm::mat4 view{1.f};
		alignas(16) glm::mat4 inverseView{1.f};
		alignas(16) glm::vec4 ambientLightColor{1.f, 1.f, 1.f, .02f}; //w is intensity
		PointLight pointLight[MAX_LIGHTS];
		int numLights;
		int debugMode;
	};

	struct FrameInfo {
		int frameIndex;
		float deltaTime;
		VkCommandBuffer commandBuffer;
		VeCamera& camera;
		VkDescriptorSet globalDescriptorSet;
		VeGameObject::Map& gameObjects;
	};

}