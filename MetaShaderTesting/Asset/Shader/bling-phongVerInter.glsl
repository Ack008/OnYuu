#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec4 aColor;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aNormal;
uniform mat4 u_modelMatrix;


out vec4 color;
out vec2 texCoord;
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
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;
float strenght = .1;
void main() {
    vec4 worldPos = u_modelMatrix * vec4(aPos, 1.0);
    vec4 vWorldPos = (u_view *  worldPos);
    texCoord = aTexCoord;
    gl_Position = u_projection * vWorldPos;
  

    vec3 result = vec3(0);
    for(int i = 0; i < count; i++)
    {
        
        Light light = lights[i];
        vec4 eyePosition = vWorldPos;
        vec4 eyeLightPos = u_view * light.position;
        vec3 N = normalize(mat3(transpose(inverse(u_view * u_modelMatrix))) * aNormal);
        vec3 V = normalize(u_position.xyz - eyePosition.xyz);
        vec3 L = normalize(eyeLightPos.xyz - eyePosition.xyz);
        vec3 R = reflect(-L, N);
        // Ambient
        vec3 ambient = strenght * material.ambient * light.intensity;
        // Diffuse
        float coseno_angolo_theta = max(dot(L, N), 0);

        vec3 diffuse = light.intensity * vec3(light.color) * coseno_angolo_theta * material.diffuse;
        // Specular
        vec3 H = normalize(L + V);
        float coseno_angolo_alfa = pow(max(dot(N, H), 0), material.shininess);

        vec3 specular = light.intensity * vec3(light.color) * coseno_angolo_alfa * material.specular;
        result += ambient + diffuse + specular;
    }
    // Solo la prima luce, come nel tuo esempio

    color = vec4(clamp(result, 0.0, 1.0), 1.0);
}