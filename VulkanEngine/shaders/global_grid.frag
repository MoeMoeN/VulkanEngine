#version 450

layout(location = 0) in vec2 fragOffset;
layout(location = 1) in vec3 fragWorldPosition;

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
    int debugMode;
} ubo;

layout(push_constant) uniform Push {
//  vec4 position;
//  vec4 color;
    float thickness;
} push;

const float M_PI = 3.1415926538;
const float GRID_CELL_SIZE = 1.0;
const float GRID_THICKNESS = 0.001;
const float DISTANCE_FALLOFF = 0.1;
const float GRID_SMOOTHNESS = 0.0025;

float simpleGrid(vec3 worldPos, float gs, float gt){
    return
    clamp(
        step(fract(gs*worldPos.x), gt) +
        step(1.0 - gt, fract(gs*worldPos.x)) +
        step(fract(gs*worldPos.z), gt) +
        step(1.0 - gt, fract(gs*worldPos.z)),
        0,
        1
    );
}

float grid(vec3 worldPos, float gs, float gt, float sm){
    float gridx1 = smoothstep(gt+sm, gt, fract(gs*worldPos.x));
    float gridx2 = smoothstep(gt+sm, gt, 1.0 - fract(gs*worldPos.x));

    float gridz1 = smoothstep(gt+sm, gt, fract(gs*worldPos.z));
    float gridz2 = smoothstep(gt+sm, gt, 1.0 - fract(gs*worldPos.z));

    return clamp(gridx1+gridx2+gridz1+gridz2, 0, 1);
}

float xLine(vec3 worldPos, float thickness, float sm){
    return smoothstep(thickness+sm, thickness, abs(worldPos.x));
}
float zLine(vec3 worldPos, float thickness, float sm){
    return smoothstep(thickness+sm, thickness, abs(worldPos.z));
}

/*

*/

void main(){
    //calculate common parameters
    vec3 cameraWorldPosition = ubo.inverseViewMatrix[3].xyz;
    float distanceToFrag = length(cameraWorldPosition - fragWorldPosition);

    //scaling with distance
    float distanceScaling = clamp(distanceToFrag/10.0, 0, 1);
    float GridThicknessWithScaling = mix(GRID_THICKNESS*0.1, GRID_THICKNESS*2.0, distanceScaling);
    float GridSmoothnessWithScaling = mix(GRID_SMOOTHNESS*0.1, GRID_SMOOTHNESS*2.0, distanceScaling);

    //make simple grids
    float gridUnit = grid(fragWorldPosition, GRID_CELL_SIZE, GridThicknessWithScaling, GridSmoothnessWithScaling);
    float gridSubUnit = grid(fragWorldPosition, 10.0*GRID_CELL_SIZE, 5.0*GridThicknessWithScaling, 10.0*GridSmoothnessWithScaling);

    //make global lines
    float xline = xLine(fragWorldPosition, GridThicknessWithScaling*1.0, GridSmoothnessWithScaling*2);
    float zline = clamp(zLine(fragWorldPosition, GridThicknessWithScaling*1.0, GridSmoothnessWithScaling*2) - xline, 0, 1); //-xline so there's no overlap

    //discard when no grid
    if (gridSubUnit+gridUnit+xline+zline < 0.001){
        discard;
    }

    //make grids to not overlapp (subtract bigger from smaller)
    gridSubUnit -= gridUnit;
    gridSubUnit -= xline;
    gridSubUnit -= zline;
    gridSubUnit = clamp(gridSubUnit, 0, 1);
    gridUnit -= xline*1.5;//*1.5 for better contrast between the two
    gridUnit -= zline*1.5;
    gridUnit = clamp(gridUnit, 0, 1);

    //make alpha for grids (with distance falloff)
    float gridUnitAlpha = gridUnit;
    float gridSubUnitAlpha = gridSubUnit;
    float xlineAlpha = xline;
    float zlineAlpha = zline;
    xlineAlpha *= 1.0 - clamp(DISTANCE_FALLOFF/4.0 * distanceToFrag, 0, 1);
    zlineAlpha *= 1.0 - clamp(DISTANCE_FALLOFF/4.0 * distanceToFrag, 0, 1);
    gridUnitAlpha *= 1.0 - clamp(DISTANCE_FALLOFF/2.0 * distanceToFrag, 0, 1);
    gridSubUnitAlpha *= 1.0 - clamp(DISTANCE_FALLOFF * distanceToFrag, 0, 1);

    //define colors for grids
    vec3 gridUnitColor = vec3(0.3, 0.3, 0.3);
    vec3 gridSubUnitColor = vec3(0.15, 0.15, 0.15);
    vec3 xlineColor = vec3(0.5, 0.05, 0.05);
    vec3 zlineColor = vec3(0.05, 0.05, 0.5);

    //colorize grids
    gridUnitColor *= gridUnit;
    gridSubUnitColor *= gridSubUnit;
    xlineColor *= xline;
    zlineColor *= zline;

    //add grids together to compose final grid.
    outColor = vec4(gridUnitColor+gridSubUnitColor+xlineColor+zlineColor, .7*(gridUnitAlpha+gridSubUnitAlpha+xlineAlpha+zlineAlpha));

}
