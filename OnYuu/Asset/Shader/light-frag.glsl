#version 450 core

in vec3 vNormal;
in vec3 vWorldPos;

out vec4 color;

struct Light {
    vec4 position;
    float intensity;
    vec4 color;
};

layout(std140, binding = 1) uniform lightsInfo {
    float count;
    Light lights[128];
};

layout(std140, binding = 2) uniform CameraInfo {
    mat4 u_view;
    mat4 u_projection;
    vec4 u_position; // camera world position
};

struct Material {
    vec3 lightColor;
};

uniform Material material;
void main() {

    color = vec4(material.lightColor,1);
}