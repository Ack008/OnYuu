#version 450 core

in vec3 vNormal;
in vec3 vWorldPos;
in vec3 reflectedVector;
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
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;
float strenght = 1.0;
uniform samplerCube skybox;
void main() {
    vec3 N = normalize(vNormal);
    vec3 viewPos = u_position.xyz;
    vec3 V = normalize(viewPos - vWorldPos);

    vec3 result = vec3(0);
    for(int i = 0; i < count; i++)
    {
        Light light = lights[i];
        vec4 eyePosition = vec4(vWorldPos,1.0);
        vec4 eyeLightPos = u_view * light.position;
        vec3 N = normalize(vNormal);
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
    vec3 reflectedColor = texture(skybox, reflectedVector).rgb;
    color = mix(color, vec4(reflectedColor, 1.0), 0.5);

}