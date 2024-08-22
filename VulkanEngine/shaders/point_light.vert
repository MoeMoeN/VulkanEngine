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
  vec4 position;
  vec4 color;
  float radius;
} push;

const float LIGHT_RADIUS = 0.05;

void main() {

    //alternative solution (better,  shorter)
    //fragOffset = OFFSETS[gl_VertexIndex];
    //vec4 lightInCameraSpace = ubo.view * vec4(ubo.lightPosition, 1.0);
    //vec4 positionInCameraSpace = lightInCameraSpace + LIGHT_RADIUS * vec4(fragOffset, 0.0, 0.0);
    //gl_Position = ubo.projection * positionInCameraSpace;

    fragOffset = OFFSETS[gl_VertexIndex];
    vec3 cameraRightVectorWorld = {ubo.viewMatrix[0][0], ubo.viewMatrix[1][0], ubo.viewMatrix[2][0]};
    vec3 cameraUpVectorWorld = {ubo.viewMatrix[0][1], ubo.viewMatrix[1][1], ubo.viewMatrix[2][1]};

    vec3 worldSpacePosition = push.position.xyz 
    + (push.radius * fragOffset.x * cameraRightVectorWorld)
    + (push.radius * fragOffset.y * cameraUpVectorWorld);

    gl_Position = ubo.projectionMatrix * (ubo.viewMatrix * vec4(worldSpacePosition, 1.0)); //same thing with additional ()
}