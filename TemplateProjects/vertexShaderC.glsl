#version 330 core // Versione GLSL utilizzata



layout(location = 0) in vec3 aPos;

layout(location = 1) in vec4 Color;



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

    gl_Position = u_viewProjectionMatrix * u_modelMatrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);

    // Passa il colore ricevuto dal buffer dati all'output verso il fragment shader,
    // così che poi venga interpolato e utilizzato per colorare ogni frammento.

    colore_frag = Color;
}


