#version 450

layout(location = 0) in vec2 fragOffset;

layout(location = 0) out vec4 outColor;

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

const float M_PI = 3.1415926538;
void main(){
    float dist = sqrt(dot(fragOffset, fragOffset));
    if (dist >= 1.0){
        discard;
    }
    outColor = vec4(push.color.xyz, 1.0 - 1.0 * dist);
}