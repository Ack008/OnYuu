#version 450 core // Versione GLSL utilizzata



layout(location = 0) in vec3 aPos;

layout(location = 1) in vec4 aColor;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aNormal;


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
    vec4 u_position;
};



out vec4 colore_frag;
uniform mat4 u_viewProjectionMatrix;
uniform mat4 u_modelMatrix;
void main() {
    // gl_Position è una variabile built-in di tipo vec4 nel vertex shader.
    // Essa contiene la posizione finale del vertice nello spazio delle coordinate normalizzate.
    //
    // La GPU usa gl_Position per sapere dove disegnare il vertice sullo schermo,
    // applicando successivamente la divisione prospettica e il viewport transform.
    //
    // Qui la posizione viene passata direttamente senza trasformazioni.
    vec4 worldCoord = u_modelMatrix * vec4(aPos, 1.0);
    gl_Position = u_projection * u_view * worldCoord;

    // Passa il colore ricevuto dal buffer dati all'output verso il fragment shader,
    // così che poi venga interpolato e utilizzato per colorare ogni frammento.
    colore_frag = vec4(0);
    for(int i = 0; i < count; i++){
        Light light = lights[i];
        if(light.color == vec4(0.5,0.5,0.5,1)){
            colore_frag = vec4(1);
            return;
        }
        mat3 normalMatrix = transpose(inverse(mat3(u_modelMatrix)));
        vec3 worldNormal = normalize(normalMatrix * aNormal);
        vec4 lightDir = normalize(light.position - worldCoord);
        float diff = max(dot(worldNormal, lightDir.xyz), 0.0);
        colore_frag += light.color * light.intensity * diff;

    }
    colore_frag = clamp(colore_frag, 0.0, 1.0);
    colore_frag *= aColor;
}


