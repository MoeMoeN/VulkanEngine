#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragWorldPosition;
layout(location = 2) in vec3 fragWorldSpaceNormal;
layout(location = 3) in vec3 fragLocalSpaceNormal;
layout(location = 4) in vec3 fragUV;

layout (location = 0) out vec4 outColor;

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


float calculateAttenuation(float distanceSquared, float constant, float linear, float quadratic) {
    float attenuation = 1.0 / (constant + linear * sqrt(distanceSquared) + quadratic * distanceSquared);
    return attenuation;
}

void main(){

    vec3 surfaceWorldSpaceNormal = normalize(fragWorldSpaceNormal);
    vec3 normalizedLocalNormal = normalize(fragLocalSpaceNormal);
    vec3 cameraWorldPosition = ubo.inverseViewMatrix[3].xyz;
    vec3 surfaceToViewerDirection = normalize(cameraWorldPosition - fragWorldPosition);


    switch(ubo.debugMode){
        case 0: 
            outColor = vec4(surfaceWorldSpaceNormal, 1.0);
            break;
        case 1:
            outColor = vec4(normalizedLocalNormal, 1.0);
            break;
        case 2:
            outColor = vec4(fragColor, 1.0);
            break;
        case 3:
            outColor = vec4(fragUV, 1.0);
            break;
        default:
            outColor = vec4(1.0, 1.0, 1.0, 1.0);
            break;
    }
}


