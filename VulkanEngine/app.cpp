#include "app.h"

#include "systems/simple_render_system.h"
#include "systems/point_light_render_system.h"
#include "systems/global_grid_render_system.h"
#include "ve_camera.h"
#include "ve_buffer.h"

#include "keyboard_movement_controller.h"


// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <chrono>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif



namespace ve {

	App::App() {
		globalPool = VeDescriptorPool::Builder(veDevice)
			.setMaxSets(VeSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VeSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();

		loadGameObjects();
	}

	App::~App() {}

	void App::run() {

		std::vector<std::unique_ptr<VeBuffer>> uboBuffers(VeSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < uboBuffers.size(); i++) {
			uboBuffers[i] = std::make_unique<VeBuffer>(
				veDevice,
				sizeof(GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			);
			uboBuffers[i]->map();
		}

		auto globalSetLayout = VeDescriptorSetLayout::Builder(veDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.build();

		std::vector<VkDescriptorSet> globalDescriptorSets(VeSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++) {
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			VeDescriptorWriter(*globalSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);
		}


		SimpleRenderSystem simpleRenderSystem{ veDevice, veRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
		PointLightRenderSystem pointLightRenderSystem{ veDevice, veRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		GridRenderSystem GlobalGridRenderSystem{ veDevice, veRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		VeCamera camera{};

		//create ,,viewer object" which is like root object of camera (camera would get transformation from that object)
		auto viewerObject = VeGameObject::createGameObject();
		viewerObject.transform.translation.z = -2.5f;
		viewerObject.transform.translation.y = -.5f;
		KeyboardMovementController cameraController{};

		auto currentTime = std::chrono::high_resolution_clock::now();

		cameraController.createPrimitives(veDevice);

		while (!veWindow.shouldClose()) {
			glfwPollEvents();

			/*TODO:
			read about Game loop  https://gameprogrammingpatterns.com/game-loop.html
			https://gafferongames.com/post/fix_your_timestep/

			and upgrade out solution to be better (interpolation)
			*/
			auto newTime = std::chrono::high_resolution_clock::now();
			float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			//transform viewerObject with cameraController
			cameraController.moveInPlaneXZ(veWindow.getGLFWwindow(), deltaTime, viewerObject);
			//apply transform from viewerObject to camera
			camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

			float aspect = veRenderer.getAspectRatio();
			//camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
			camera.setPerspectiveProjection(camera.HFOVtoVFOV(glm::radians(38.5f), aspect), aspect, 0.1f, 500.0f);
			
			cameraController.transformSelectedGameObject(veWindow.getGLFWwindow(), veGameObjects, deltaTime);
			cameraController.addPrimitive(veWindow.getGLFWwindow(), veGameObjects, veDevice);

			if (auto commandBuffer = veRenderer.beginFrame()) {

		
				int frameIndex = veRenderer.getFrameIndex();
				FrameInfo frameInfo{
					frameIndex,
					deltaTime,
					commandBuffer,
					camera,
					globalDescriptorSets[frameIndex],
					veGameObjects
				};


				//update
				GlobalUbo ubo{};
				ubo.projection = camera.getProjection();
				ubo.view = camera.getView();
				ubo.inverseView = camera.getInverseView();
				pointLightRenderSystem.update(frameInfo, ubo);
				ubo.debugMode = cameraController.switchDebugMode(veWindow.getGLFWwindow());
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				//render
				veRenderer.beginSwapChainRenderPass(commandBuffer);

				// oreder here matters, opaque first for simple transparency
				simpleRenderSystem.renderGameObjects(frameInfo);
				pointLightRenderSystem.render(frameInfo);
				GlobalGridRenderSystem.render(frameInfo);

				//
				veRenderer.endSwapChainRenderPass(commandBuffer);
				veRenderer.endFrame();
			}
		}

		vkDeviceWaitIdle(veDevice.device());
	}

	void App::loadGameObjects() {

		//GLOBAL GRID ID 0
		auto globalGrid = VeGameObject::setAsGlobalGrid();
		globalGrid.transform.translation = { 0.0f, 0.f, 0.0f };
		globalGrid.transform.scale = { 1.f, 1.f, 1.f };
		globalGrid.color = { 1.f, 1.f, 1.f };

		veGameObjects.emplace(globalGrid.getId(), std::move(globalGrid));

		//GIZMO ID 1
		std::shared_ptr<VeModel> veModel = VeModel::createModelFromFile(veDevice, "models/simple_gizmo.obj");

		auto gizmoObj = VeGameObject::createGameObject();
		gizmoObj.model = veModel;
		gizmoObj.transform.translation = { -0.0f, 0.f, 0.0f };
		gizmoObj.transform.scale = { 0.f, 0.f, 0.f };
		gizmoObj.color = { 1.f, 1.f, 1.f };
		gizmoObj.pipelineID = 2; //disabled depth test pipeline

		veGameObjects.emplace(gizmoObj.getId(), std::move(gizmoObj));



		///... models ... ///
		//
		veModel = VeModel::createModelFromFile(veDevice, "models/stanford_bunny.obj");
		auto gameObj = VeGameObject::createGameObject();
		gameObj.model = veModel;
		gameObj.transform.translation = { -0.0f, 0.f, 0.0f };
		gameObj.transform.scale = { 3.f, 3.f, 3.f };
		gameObj.color = { 1.f, 1.f, 1.f };
		gameObj.pipelineID = 0;

		veGameObjects.emplace(gameObj.getId(), std::move(gameObj));

		//
		veModel = VeModel::createModelFromFile(veDevice, "models/stanford_bunny_VC.obj");
		auto gameObj2 = VeGameObject::createGameObject();
		gameObj2.model = veModel;
		gameObj2.transform.translation = { -1.0f, 0.f, 0.0f };
		gameObj2.transform.scale = { 3.f, 3.f, 3.f };
		gameObj2.color = { 1.f, 1.f, 1.f };
		gameObj2.transform.rotation = { 0.0f, 1.0f, 0.0f };
		gameObj2.pipelineID = 1; //ID 1 --- debug mode

		veGameObjects.emplace(gameObj2.getId(), std::move(gameObj2));

		/*

		veModel = VeModel::createModelFromFile(veDevice, "models/smooth_vase.obj");

		auto gameObj2 = VeGameObject::createGameObject();
		gameObj2.model = veModel;
		gameObj2.transform.translation = { 0.5f, 0.f, 0.f };
		gameObj2.transform.scale = { 3.f, 3.f, 3.f };
		gameObj2.color = { 1.f, 1.f, 1.f };

		veGameObjects.emplace(gameObj2.getId(), std::move(gameObj2));


		veModel = VeModel::createModelFromFile(veDevice, "models/quad.obj");
		
		auto floorObj = VeGameObject::createGameObject();
		floorObj.model = veModel;
		floorObj.transform.translation = { -0.0f, 0.f, 0.0f };
		floorObj.transform.scale = { 5.f, 1.f, 5.f };
		floorObj.color = { 1.f, 1.f, 1.f };

		veGameObjects.emplace(floorObj.getId(), std::move(floorObj));

		*/

		/*
		* POINT LIGHTS
		*/
		/*
		{//limit pointLight to scope --- after move() pointLight is invalid so that's a solution for that
			auto pointLight = VeGameObject::makePointLight(0.5f, 0.05f);
			pointLight.transform.translation = { 0.0f, -1.0f, 0.0f };
			veGameObjects.emplace(pointLight.getId(), std::move(pointLight));
		}
		*/
		/*
		std::vector<glm::vec3> lightColors{
			{1.f, .1f, .1f},
			{ .1f, .1f, 1.f },
			{ .1f, 1.f, .1f },
			{ 1.f, 1.f, .1f },
			{ .1f, 1.f, 1.f },
			{ 1.f, 1.f, 1.f }  //
		};
		*/
		std::vector<glm::vec3> lightColors{
			{1.f, 1.f, 1.f},
			{0.5f, 0.5f, 1.f},
			{0.5f, 1.f, 0.5f}
		};

		for (int i = 0; i < lightColors.size(); i++) { 
			auto pointLight = VeGameObject::makePointLight(0.5f, 0.02f+0.005f*i);
			pointLight.color = lightColors[i];

			auto rotateLight = glm::rotate(glm::mat4(1.f), (i * glm::two_pi<float>()) / lightColors.size(), { 0.f, -1.f, 0.f });
			pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.f, -0.5f/* *i - 0.1f*/, -1.f, -1.f));

			veGameObjects.emplace(pointLight.getId(), std::move(pointLight));
		}

	}

	std::unique_ptr<VeModel> App::createCubeGeometry(VeDevice& device, glm::vec3 offset)
	{
		VeModel::Builder modelBuilder{};
		modelBuilder.vertices = {
			// left face (white)
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},

			// right face (yellow)
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .8f, .1f}},

			// top face (orange, remember y axis points down)
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},

			// bottom face (red)
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .1f, .1f}},

			// nose face (blue)
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},

			// tail face (green)
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
		};
		for (auto& v : modelBuilder.vertices) {
			v.position += offset;
		}

		modelBuilder.indices = { 0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
								12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21 };

		return std::make_unique<VeModel>(device, modelBuilder);
	}

}