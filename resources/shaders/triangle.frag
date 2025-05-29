#version 460 core

// Creo una variabile in output (pin di uscita), di tipo vec4 per il colore, col nome da noi specificato. 

out vec4 frag_colors;

void main()
{
    // Specifichiamo il colore, per ora un rosso può andare bene.
    // Non esiste una uscita di default, ma viene identificato come colore del pixel il primo "out" dichiarato.

    frag_colors = vec4(1.f, 0.f, 0.f, 1.f);
}
