#version 460 core

// Creo un pin di ingresso in posizione 0. 
// Creo quindi una variabile con la keyword Layout, di tipo "in" input. Che conterrà 3 elementi float (vec3) con un nome scelto da noi.

layout (location = 0) in vec3 vert_pos;

void main()
{
    // Sul vertice entrante non dobbiamo fare operazioni particolari, poichè lo abbiamo fornito già in NDC.
    // Il pin di output di default di un Vertex Shader è gl_Position. Poichè questo è un vec4 dobbiamo castare il dato che arriva in ingresso. 
    // Per il quarto canale va bene per ora un 1.f fisso, è un valore "neutro".

    gl_Position = vec4(vert_pos, 1.f);
}
