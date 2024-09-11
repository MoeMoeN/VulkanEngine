#pragma once

#include "ve_model.h"

//libs
#include <glm/gtc/matrix_transform.hpp>

//std
#include <memory>
#include <unordered_map>
#include <iostream>

namespace ve {

struct TransformComponent {
	glm::vec3 translation{}; //offset
	glm::vec3 scale{1.f, 1.f, 1.f};
	glm::vec3 rotation{};

	/*glm::mat4 mat4() {
		auto transform = glm::translate(glm::mat4{1.f}, translation); //creates mat4 from identity mat4 and translation(offset)
		
		//euler Y X Z
		transform = glm::rotate(transform, rotation.y, { 0.f, 1.f, 0.f });
		transform = glm::rotate(transform, rotation.x, { 1.f, 0.f, 0.f });
		transform = glm::rotate(transform, rotation.z, { 0.f, 0.f, 1.f });

		transform = glm::scale(transform, scale); 	
		return transform;
	}
	*/
	// Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
	// https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
	glm::mat4 mat4();
	glm::mat3 normalMatrix();
};

struct PointLightComponent {
	float lightIntensity = 1.0f;
};

	/*
	* this is very simple game object system.
	* it is not optimal but easy to setup and flexible for prototyping.
	* in future we need to change that so objects that don't need some functionality will inherit(if OOP) from very base class or
	* make it as ECS(entity component system)
	*/
class VeGameObject {
public:
	using id_t = unsigned int;
	using Map = std::unordered_map<id_t, VeGameObject>;

	static VeGameObject createGameObject() {
		static id_t currentId = 0;
		std::cout << "current object ID: " << currentId << std::endl;
		return VeGameObject{ currentId++ };
	}

	static VeGameObject makePointLight(float intensity = 10.f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.f));
	static VeGameObject setAsGlobalGrid();

	VeGameObject(const VeGameObject&) = delete;
	VeGameObject &operator=(const VeGameObject&) = delete;
	VeGameObject(VeGameObject&&) = default;
	VeGameObject &operator=(VeGameObject&&) = default;




	id_t getId() const { return id; };

	glm::vec3 color{};
	TransformComponent transform{};
	
	bool isGlobalGrid = false;
	int pipelineID = 0;

	//Optional pointer components
	std::shared_ptr<VeModel> model{};
	std::unique_ptr<PointLightComponent> pointLight = nullptr;

private:
	VeGameObject(id_t objId) : id{ objId } {}

	id_t id;
};
}