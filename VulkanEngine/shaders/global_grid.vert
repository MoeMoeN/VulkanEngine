#version 450

const vec2 OFFSETS[6] = vec2[](
  vec2(-1.0, -1.0),
  vec2(-1.0, 1.0),
  vec2(1.0, -1.0),
  vec2(1.0, -1.0),
  vec2(-1.0, 1.0),
  vec2(1.0, 1.0)
);

layout (location = 0) out vec2 fragOffset;
layout (location = 1) out vec3 fragWorldPosition;

struct PointLight{
  vec4 position; //ignore w 
  vec4 color; //w is intensity
};

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projectionMatrix;
    mat4 viewMatrix;
    mat4 inverseViewMatrix;
    vec4 ambientLightColor; //w is intenstiy
    PointLight pointLights[10]; //hardcoded 10 can be changed using specialization constants
    int numLights;
} ubo;

layout(push_constant) uniform Push {
//  vec4 position;
//  vec4 color;
    float thickness;
} push;

const float GRID_SIZE = 500.0;

void main() {

    //alternative solution (better,  shorter)
    //fragOffset = OFFSETS[gl_VertexIndex];
    //vec4 lightInCameraSpace = ubo.view * vec4(ubo.lightPosition, 1.0);
    //vec4 positionInCameraSpace = lightInCameraSpace + LIGHT_RADIUS * vec4(fragOffset, 0.0, 0.0);
    //gl_Position = ubo.projection * positionInCameraSpace;

    fragOffset = OFFSETS[gl_VertexIndex];
    vec3 cameraRightVectorWorld = {ubo.viewMatrix[0][0], ubo.viewMatrix[1][0], ubo.viewMatrix[2][0]};
    vec3 cameraUpVectorWorld = {ubo.viewMatrix[0][1], ubo.viewMatrix[1][1], ubo.viewMatrix[2][1]};

    vec3 worldSpacePosition = vec3(0.0, -0.0025, 0.0) //small offset in Y to prevent Z fighting with ground plane
    + (GRID_SIZE * fragOffset.x * vec3(1.0, 0.0, 0.0))
    + (GRID_SIZE * fragOffset.y * vec3(0.0, 0.0, 1.0)); //ground plane XZ

    fragWorldPosition = worldSpacePosition;
    gl_Position = ubo.projectionMatrix * (ubo.viewMatrix * vec4(worldSpacePosition, 1.0)); //same thing with additional ()
}