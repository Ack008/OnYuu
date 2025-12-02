#version 330 core
layout (location = 0) in vec3 aPos;   // the position variable has attribute position 0
layout (location = 1) in vec4 aColor; // the color variable has attribute position 1
layout (location = 2) in vec2 aTex; // the color variable has attribute position 1

layout(location = 3) in vec3 vertexNormal;  //Attributo normale 2

  
out vec4 color; // output a color to the fragment shader
uniform mat4 Projection;  //vARIABILE DI TIPO uniform, rimane la stessa per ogni vertice della primitiva e viene passata dall'esterno
uniform mat4 Model;
uniform mat4 View;
uniform vec3 ViewPos;

//Struttura per la gestione di un punto luce
struct Light {
	vec3 position;
	vec3 color;
	float power;
};
//definizione di una variabile uniform che ha la struttura PointLight
uniform Light light;

//Struttura per la gestione di un materiale

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

//Variabile uniorme
uniform Material material;

float strenght = 1.0;
void main()
{
//Trasformazione dei vertici dalle coordinate nel sistema di riferimento dell'oggetto (apos), al sistema di riferimento del mondo (premoltiplicazione 
// per Model) e successivamente proiettate nel cubo di centro l'origine e lato lungo 2, con x,y,z che variano tra -1 ed 1- (premoltiplicazione 
//per la matrice Projection)
        gl_Position = Projection * View * Model * vec4(aPos, 1.0);

        //Trasformare le coordinate del vertice da elaborare (aPos) in coordinate di vista
        vec4 eyePosition = View * Model * vec4(aPos, 1.0);

        //Trasformiamo la posizione della luce nelle coordinate di vista

        vec4 eyeLightPos = View * vec4(light.position, 1.0);

        //trasformare le normali nel vertice in esame nel sistema di coordinate di vista

        vec3 N = normalize(transpose(inverse(mat3(View * Model))) * vertexNormal);
        //Calcoliamo la direzione della luce L, la direzione riflessione R e di vista

        vec3 V = normalize(ViewPos - eyePosition.xyz);
        vec3 L = normalize((eyeLightPos - eyePosition).xyz);
        vec3 R = reflect(-L, N);  //Costruisce la direzione riflessa di L rispesso alla normale

        //ambientale
        vec3 ambient = strenght * light.power * material.ambient;

        //diffuse
        float coseno_angolo_theta = max(dot(L, N), 0);

        vec3 diffuse = light.power * light.color * coseno_angolo_theta * material.diffuse;

        //speculare
        float coseno_angolo_alfa = pow(max(dot(V, R), 0), material.shininess);

        vec3 specular = light.power * light.color * coseno_angolo_alfa * material.specular;

        color = vec4(ambient + diffuse + specular, 1.0);
}  

