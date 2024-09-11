#include "keyboard_movement_controller.h"

#include <limits>
#include <iostream>

namespace ve {

void ve::KeyboardMovementController::moveInPlaneXZ(GLFWwindow* window, float deltaTime, VeGameObject& gameObject) {
	
	glm::vec3 rotate{0};

	if (glfwGetKey(window, keys.lookRight) == GLFW_PRESS) rotate.y += 1.f;
	if (glfwGetKey(window, keys.lookLeft) == GLFW_PRESS) rotate.y -= 1.f;
	if (glfwGetKey(window, keys.lookUp) == GLFW_PRESS) rotate.x += 1.f;
	if (glfwGetKey(window, keys.lookDown) == GLFW_PRESS) rotate.x -= 1.f;

	if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
		gameObject.transform.rotation += lookSpeed * deltaTime * glm::normalize(rotate);
	}

	//limit pitch values between about +/- 85ish degrees
	gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x, -1.5f, 1.5f);
	gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());

	float yaw = gameObject.transform.rotation.y;
	const glm::vec3 forwardDir{sin(yaw), 0.f, cos(yaw)};
	const glm::vec3 rightDir{forwardDir.z, 0.f, -forwardDir.x};
	const glm::vec3 upDir{0.f, -1.f, 0.f};

	glm::vec3 moveDir{0.f};
	if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS) moveDir += forwardDir;
	if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS) moveDir -= forwardDir;
	if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS) moveDir += rightDir;
	if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) moveDir -= rightDir;
	if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS) moveDir += upDir;
	if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS) moveDir -= upDir;

	if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
		gameObject.transform.translation += moveSpeed * deltaTime * glm::normalize(moveDir);
	}
}

int KeyboardMovementController::switchDebugMode(GLFWwindow* window)
{
	int currentKeyState = glfwGetKey(window, GLFW_KEY_SPACE);

	// Check if the space key was pressed (transition from RELEASE to PRESS)
	if (currentKeyState == GLFW_PRESS && previousKeyState == GLFW_RELEASE) {
		debugModeID++;  // Increment the counter only on press
		if (debugModeID > 3) { //hard coded 3 debug modes
			debugModeID = 0;
		}
	}

	// Update the previous key state
	previousKeyState = currentKeyState;
	return debugModeID;
}

void KeyboardMovementController::transformSelectedGameObject(GLFWwindow* window, VeGameObject::Map& veGameObjects, float deltaTime)
{
	int lightNumber = 3 + 1;
	int objSize = veGameObjects.size();// -1;// -lightNumber;
	//std::cout << objSize << std::endl;
	float speed = 1.5f;

	int currentKeyState = glfwGetKey(window, GLFW_KEY_PAGE_UP);

	//get gizmo
	auto itg = veGameObjects.find(1);
	auto& gizmo = itg->second;

	// Check if the space key was pressed (transition from RELEASE to PRESS)
	if (currentKeyState == GLFW_PRESS && previousKeyStateSelection == GLFW_RELEASE) {
		//std::cout << "selectedObjectIncrement from: " << selectedObjectID;
		selectedObjectID++;  // Increment the counter only on press
		//std::cout << " -> to -> " << selectedObjectID << std::endl;
		if (selectedObjectID < 2 ) {
			selectedObjectID = 2;
		}
		if (selectedObjectID > objSize) {
			selectedObjectID = -1;
			gizmo.transform.scale = { 0.0f, 0.0f, 0.0f };
		}
		//TO FIX LATER: jump to next on broken selection
		if (selectedObjectID == 7) { //magic number == number of initially loaded objects.
			selectedObjectID++;
		}
	}

	// Update the previous key state
	previousKeyStateSelection = currentKeyState;

	auto it = veGameObjects.find(selectedObjectID);
	if (it != veGameObjects.end()) {
		auto& gameObject = it->second;
		std::cout << "selected game object id" << gameObject.getId() << std::endl;
		std::cout << "selected game object position: " << gameObject.transform.translation.x << ", "
			<< gameObject.transform.translation.y << ", "
			<< gameObject.transform.translation.z << ", ";


		// Update the transform
		//scale
		if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS) {
			gameObject.transform.scale += speed * deltaTime;
		}

		if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS) {
			gameObject.transform.scale -= speed * deltaTime;
		}
		
		//translation
		if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
			gameObject.transform.translation.x += speed * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
			gameObject.transform.translation.x -= speed * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
			gameObject.transform.translation.z += speed * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
			gameObject.transform.translation.z -= speed * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
			gameObject.transform.translation.y += speed * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
			gameObject.transform.translation.y -= speed * deltaTime;
		}

		//rotation
		if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
			gameObject.transform.rotation.x += speed * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
			gameObject.transform.rotation.x -= speed * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
			gameObject.transform.rotation.z += speed * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
			gameObject.transform.rotation.z -= speed * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
			gameObject.transform.rotation.y += speed * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
			gameObject.transform.rotation.y -= speed * deltaTime;
		}

		//match gizmo with the selected object.
		gizmo.transform.translation = gameObject.transform.translation;
		gizmo.transform.scale = gameObject.transform.scale/2.0f;
		gizmo.transform.rotation = gameObject.transform.rotation;

	}
}
void KeyboardMovementController::addPrimitive(GLFWwindow* window, VeGameObject::Map& veGameObjects, VeDevice& device)
{
	int currentKeyState = false;
	std::shared_ptr<VeModel> primitive = nullptr;
	if (glfwGetKey(window, GLFW_KEY_1)) {
		currentKeyState = true;
		primitive = Primitive1;
	}
	if (glfwGetKey(window, GLFW_KEY_2)) {
		currentKeyState = true;
		primitive = Primitive2;
	}
	if (glfwGetKey(window, GLFW_KEY_3)) {
		currentKeyState = true;
		primitive = Primitive3;
	}
	if (glfwGetKey(window, GLFW_KEY_4)) {
		currentKeyState = true;
		primitive = Primitive4;
	}
	if (glfwGetKey(window, GLFW_KEY_5)) {
		currentKeyState = true;
		primitive = Primitive5;
	}
	if (glfwGetKey(window, GLFW_KEY_6)) {
		currentKeyState = true;
		primitive = Primitive6;
	}
	if (glfwGetKey(window, GLFW_KEY_7)) {
		currentKeyState = true;
		primitive = Primitive7;
	}
	if (glfwGetKey(window, GLFW_KEY_8)) {
		currentKeyState = true;
		primitive = Primitive8;
	}
	if (glfwGetKey(window, GLFW_KEY_9)) {
		currentKeyState = true;
		primitive = Primitive9;
	}

	 
	//get gizmo
	auto itg = veGameObjects.find(1);
	auto& gizmo = itg->second;
	

	// Check if the space key was pressed (transition from RELEASE to PRESS)
	if (currentKeyState == GLFW_PRESS && previousKeyStatePrimitive == GLFW_RELEASE) {
		//std::shared_ptr<VeModel> veModel = VeModel::createModelFromFile(device, "models/stanford_bunny.obj");//createCubeGeometry(device, glm::vec3(0.0f));
		auto gameObj = VeGameObject::createGameObject();
		gameObj.model = primitive;
		gameObj.transform.translation = { -0.0f, 0.f, 0.0f };
		gameObj.transform.scale = { 1.f, 1.f, 1.f };
		gameObj.color = { 1.f, 1.f, 1.f };
		gameObj.pipelineID = 0;



		auto objId = gameObj.getId();
		veGameObjects.emplace(gameObj.getId(), std::move(gameObj));
		
		// Verify if the object was successfully added
		auto it = veGameObjects.find(objId);
		if (it != veGameObjects.end()) {
			auto& addedObj = it->second;
			std::cout << "Added object ID: " << objId << " at position: "
				<< addedObj.transform.translation.x << ", "
				<< addedObj.transform.translation.y << ", "
				<< addedObj.transform.translation.z << std::endl;
			selectedObjectID = objId;
		}
		else {
			std::cerr << "Failed to add object with ID: " << objId << std::endl;
		}
	}

	// Update the previous key state
	previousKeyStatePrimitive = currentKeyState;
}
void KeyboardMovementController::createPrimitives(VeDevice& device)
{
	Primitive1 = VeModel::createModelFromFile(device, "models/prim_quad.obj");
	Primitive2 = VeModel::createModelFromFile(device, "models/prim_cube.obj");
	Primitive3 = VeModel::createModelFromFile(device, "models/prim_sphere.obj");
	Primitive4 = VeModel::createModelFromFile(device, "models/prim_icosphere.obj");
	Primitive5 = VeModel::createModelFromFile(device, "models/prim_cylinder.obj");
	Primitive6 = VeModel::createModelFromFile(device, "models/prim_cone.obj");
	Primitive7 = VeModel::createModelFromFile(device, "models/prim_torus.obj");
	Primitive8 = VeModel::createModelFromFile(device, "models/stanford_dragon.obj");
	Primitive9 = VeModel::createModelFromFile(device, "models/prim_monk.obj");
}
std::unique_ptr<VeModel> KeyboardMovementController::createCubeGeometry(VeDevice& device, glm::vec3 offset)
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