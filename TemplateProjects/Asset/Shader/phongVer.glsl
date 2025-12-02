#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aNormal;

uniform mat4 u_modelMatrix;
out vec3 vNormal;
out vec3 vWorldPos;
out vec3 vColor;
out vec2 vTexCoord;
layout(std140, binding = 2) uniform CameraInfo {
    mat4 u_view;
    mat4 u_projection;
    vec4 u_position; // camera world position
};

void main()
{
    mat4 model = mat4(1.0);
    vNormal = mat3(transpose(inverse(u_view * u_modelMatrix))) * aNormal; 
    vec4 worldPos = u_modelMatrix * vec4(aPos, 1.0);
    vWorldPos = (u_view *  worldPos).xyz;
    vColor = aColor;
    vTexCoord = aTexCoord;
    gl_Position = u_projection * u_view * worldPos ;
}