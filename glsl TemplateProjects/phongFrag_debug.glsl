#version 450 core

in vec3 vWorldPos;
out vec4 color;

layout(std140, binding = 2) uniform CameraInfo {
    mat4 u_view;
    mat4 u_projection;
    vec4 u_position;
};

void main() {
    // NaN check
    if (any(isnan(u_position.xyz)) || any(isnan(vWorldPos))) {
        color = vec4(1.0, 0.0, 1.0, 1.0); // magenta = dati corrotti
        return;
    }

    float d = length(u_position.xyz - vWorldPos);

    // mappa distanza: near -> nero, far -> bianco (scelta inversa della precedente)
    float nd = clamp(d / 10.0, 0.0, 1.0); // regola 10.0 come prefisso
    color = vec4(vec3(nd), 1.0);

    // Per invertire (near bianco, far nero): color = vec4(vec3(1.0 - nd), 1.0);
}