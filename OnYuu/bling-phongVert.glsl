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





struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

//Variabile uniorme
uniform Material material;

out vec4 colore_frag;
uniform mat4 u_modelMatrix;
float strenght = 1.0;
void main() {
    // gl_Position è una variabile built-in di tipo vec4 nel vertex shader.
    // Essa contiene la posizione finale del vertice nello spazio delle coordinate normalizzate.
    //
    // La GPU usa gl_Position per sapere dove disegnare il vertice sullo schermo,
    // applicando successivamente la divisione prospettica e il viewport transform.
    //
    // Qui la posizione viene passata direttamente senza trasformazioni.
    vec4 worldCoord = u_modelMatrix * vec4(aPos, 1.0);
    vec4 eyePosition = u_view * worldCoord;
    gl_Position = u_projection * eyePosition;

    // Passa il colore ricevuto dal buffer dati all'output verso il fragment shader,
    // così che poi venga interpolato e utilizzato per colorare ogni frammento.
    colore_frag = vec4(0);
        Light light = lights[0];
        vec4 eyeLightPos = u_view * light.position;
        vec3 N = normalize(transpose(inverse(mat3(u_view * u_modelMatrix))) * aNormal);
        vec3 viewPos = vec3(u_position);  
        vec3 V = normalize(viewPos - eyePosition.xyz);
        vec3 L = normalize((eyeLightPos - eyePosition).xyz);
        vec3 R = reflect(-L, N);  //Costruisce la direzione riflessa di L rispesso alla normale
        //ambientale
        vec3 ambient = strenght * light.intensity * material.ambient;

        //diffuse
        float coseno_angolo_theta = max(dot(L, N), 0);

        vec3 diffuse = light.intensity * vec3(light.color) * coseno_angolo_theta * material.diffuse;

        //speculare
        float coseno_angolo_alfa = pow(max(dot(V, R), 0), material.shininess);

        vec3 specular = light.intensity * vec3(light.color) * coseno_angolo_alfa * material.specular;
        colore_frag += vec4(ambient + diffuse + specular, 1.0);
    colore_frag = clamp(colore_frag, 0.0, 1.0);
}


