#pragma once

#include "ve_game_object.h"
#include "ve_window.h"

namespace ve {
class KeyboardMovementController {
public:
	struct KeyMappings {
        int moveLeft = GLFW_KEY_A;
        int moveRight = GLFW_KEY_D;
        int moveForward = GLFW_KEY_W;
        int moveBackward = GLFW_KEY_S;
        int moveUp = GLFW_KEY_E;
        int moveDown = GLFW_KEY_Q;
        int lookLeft = GLFW_KEY_LEFT;
        int lookRight = GLFW_KEY_RIGHT;
        int lookUp = GLFW_KEY_UP;
        int lookDown = GLFW_KEY_DOWN;
	};


    void moveInPlaneXZ(GLFWwindow* window, float deltaTime, VeGameObject& gameObject);
    int switchDebugMode(GLFWwindow* window);
    void transformSelectedGameObject(GLFWwindow* window, VeGameObject::Map& veGameObjects, float deltaTime);
    void addPrimitive(GLFWwindow* window, VeGameObject::Map& veGameObjects, VeDevice& device);
    void createPrimitives(VeDevice& device);

    std::unique_ptr<VeModel> createCubeGeometry(VeDevice& device, glm::vec3 offset);
    int previousKeyStatePrimitive = GLFW_RELEASE;

    KeyMappings keys{};
    float moveSpeed{ 3.f };
    float lookSpeed{ 1.5f };

    using id_t = unsigned int;
    id_t selectedObjectID = -1;
    int previousKeyStateSelection = GLFW_RELEASE;
    bool initial_empty_spawn = false;
    
    int debugModeID = -1;
    int previousKeyState = GLFW_RELEASE;

    std::shared_ptr<VeModel> Primitive1;
    std::shared_ptr<VeModel> Primitive2;
    std::shared_ptr<VeModel> Primitive3;
    std::shared_ptr<VeModel> Primitive4;
    std::shared_ptr<VeModel> Primitive5;
    std::shared_ptr<VeModel> Primitive6;
    std::shared_ptr<VeModel> Primitive7;
    std::shared_ptr<VeModel> Primitive8;
    std::shared_ptr<VeModel> Primitive9;
};

}