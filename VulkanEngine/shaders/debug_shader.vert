#version 450

layout(location = 0) in vec3 position; 
layout(location = 1) in vec3 vertColor;
layout(location = 2) in vec3 vertNormal;
layout(location = 3) in vec3 uv;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragWorldPosition;
layout(location = 2) out vec3 fragWorldSpaceNormal;
layout(location = 3) out vec3 fragLocalSpaceNormal;
layout(location = 4) out vec3 fragUV;

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
    int debugMode;
} ubo;

layout(push_constant) uniform Push {
    mat4 modelMatrix; 
    mat4 normalMatrix; 
} push;

void main(){
    //position calculations
    vec4 worldPosition = push.modelMatrix * vec4(position, 1.0);
    gl_Position = ubo.projectionMatrix * (ubo.viewMatrix * worldPosition); //() for performance reasons -> avoid unessesary matrix*matrix calculation which is heavy

    vec3 normalWorldSpace = normalize(mat3(push.normalMatrix) * vertNormal);

    fragColor = vertColor;
    fragWorldPosition = worldPosition.xyz;
    fragWorldSpaceNormal = normalWorldSpace;
    fragLocalSpaceNormal = vertNormal;
    fragUV = uv;
}