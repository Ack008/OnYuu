#version 330 core

// colore_frag:  Variabile di input (interpolata) proveniente dal vertex shader.
// Per ogni pixel (frammento) questa contiene il colore calcolato e interpolato tra i vertici del triangolo.

in vec4 colore_frag;

// color: Variabile di output che rappresenta il colore finale che sarà scritto sul framebuffer (schermo).
out vec4 color;

void main() {
    // Qui viene assegnato al colore finale del pixel il valore ricevuto dal vertex shader.
    // Poiché non viene fatto altro calcolo o modifica, il colore viene semplicemente passato così com'è.
    // Questo significa che ogni pixel mostra il colore interpolato tra i colori dei vertici.
    color = colore_frag;
}
