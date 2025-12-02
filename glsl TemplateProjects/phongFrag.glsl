#version 450 core

in vec3 vWorldPos;
out vec4 color;

layout(std140, binding = 2) uniform CameraInfo {
    mat4 u_view;
    mat4 u_projection;
    vec4 u_position;
};

void main() {
    // distanza camera -> frammento
    float d = length(u_position.xyz - vWorldPos);

    // mappa distanza in [0,1] per visualizzare (0 = nero, 1 = bianco)
    float nd = clamp(d / 10.0, 0.0, 1.0);
    color = vec4(vec3(nd), 1.0);

    // Per vedere componenti: color = vec4(abs(u_position.xyz) / 10.0, 1.0);
}