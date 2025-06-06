#version 460 core

// Pin per la location che identifica il vertice. Rimane invariato.
layout (location = 0) in vec3 vert_pos;

// Nella posizione 1 invece, esponiamo un nuovo pin input (in) vec3 per l'identificazione del colore.
// Usiamo un vec3 perchè nei nostri dati (esempio Ex04) non abbiamo l'Alpha come identificativo.
layout (location = 1) in vec3 vert_col;

// Prepariamo un pin di uscita, che permetterà l'interpolazione della GPU per colorare correttamente i pixel.
// Ricordo infatti che il .vert non accede direttamente ai pixel, ma lavora sui vertici.
out vec4 vert_col_out;

// Creiamo una uniform che conterrà l'informazione che useremo per scalare i vertici.
uniform float scale;

void main()
{
    // Scaliamo i vertici entranti.
    vec3 vert_pos_scaled = vert_pos * scale;

    // Posizione vertice.
    // gl_Position = vec4(vert_pos, 1.f);
    gl_Position = vec4(vert_pos_scaled, 1.f); // Attenzione che l'ultimo 1 non deve essere scalato.

    // Impostiamo il colore vertice (out). Castiamo a vec4 per il canale Alpha, hardcodato ad 1 per l'esempio.
    vert_col_out = vec4(vert_col, 1.f);
}
