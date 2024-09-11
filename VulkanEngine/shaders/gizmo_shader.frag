#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragWorldPosition;
layout(location = 2) in vec3 fragWorldSpaceNormal;

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

    vec3 diffuseLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
    vec3 specularLight = vec3(0.0);
    vec3 surfaceWorldSpaceNormal = normalize(fragWorldSpaceNormal);

    vec3 cameraWorldPosition = ubo.inverseViewMatrix[3].xyz;
    vec3 surfaceToViewerDirection = normalize(cameraWorldPosition - fragWorldPosition);

    outColor = vec4(fragColor, 1.0);
    // for (int i = 0; i < ubo.numLights; i++){
    //     PointLight light = ubo.pointLights[i];
    //     vec3 directionToLight = light.position.xyz - fragWorldPosition;

    //     float distanceSquared = dot(directionToLight, directionToLight);
    //     float attenuation = calculateAttenuation(distanceSquared, 1.0, 0.09, 0.032);
        
    //     directionToLight = normalize(directionToLight);

    //     float cosAngIncidence = max(dot(surfaceWorldSpaceNormal, directionToLight), 0);

    //     vec3 intensity = light.color.xyz * light.color.w * attenuation;

    //     diffuseLight += intensity * cosAngIncidence;

    //     //spec
    //     vec3 halfAngle = normalize(directionToLight + surfaceToViewerDirection);
    //     float blinnTerm = dot(surfaceWorldSpaceNormal, halfAngle);
    //     blinnTerm = max(blinnTerm, 0);
    //     blinnTerm = pow(blinnTerm, 32.0); // higher values -> sharper highlight
    //     specularLight += intensity * blinnTerm;
    // }
    //spec multiplied by fragColor to imitate metalic materials (that should be parameter)
    //outColor = vec4(diffuseLight * fragColor + specularLight * fragColor, 1.0);
    //outColor = vec4(attenuation.xxx, 1.0);
}


