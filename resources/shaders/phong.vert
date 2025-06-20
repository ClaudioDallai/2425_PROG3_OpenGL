#version 460 core

layout (location = 0) in vec3 vert_pos;
layout (location = 1) in vec2 vert_uv;
layout (location = 2) in vec3 vert_normal;

uniform mat4 mvp; // Uniform matrice per la MVP.
uniform mat4 model;

out vec2 vert_uv_out;
out vec3 vert_world_pos_out;
out vec3 vert_world_norm_out;

void main()
{
    // Applico la MVP ad ogni vertice, poichè la matrice è 4x4 devo inserire un ulteriore 1, che fa in modo che tutto funzioni essendo neutro.
    gl_Position = mvp * vec4(vert_pos, 1.f);

    vert_uv_out = vert_uv;

    // Moltiplicando model per i vec3, questi diventano in coordinate mondo grazie alle trasformazioni delle matrici.
    vert_world_pos_out = (model * vec4(vert_pos, 1.f)).xyz;
    // vert_world_norm_out = (model * vec4(vert_normal, 1.f)).xyz;

    /* 
       Evitiamo problemi di scale non lineare per le normali e di traslazione.
       Inverto la matrice. Lo scale sta sulla diagonale, e la traslazione sull'ultima colonna. Nel 3x3 della amtrice si troveranno eventuali rotazioni.
       Facendo "inverse" però, invertiamo tutto, compreso rotazioni e traslazione. La traslazione per le normali non ci deve proprio interessare.
       L'inverso aggiusta lo scale, ma dobbiamo ri-mettere apposto le rotazioni.
       Una proprietà della matrice di rotazione dice che la trasposta dell'inversa della matrice permette l'ottenere la matrice di rotazione originaria.
       Questa proprietà ("transpose") agisce solo sulle rotazioni nella matrice.
       Essendo che la traslazione non serve proprio per le normali, salviamo il risultato in una mat3 escludendo proprio le traslazioni, che sta nell'ultima colonna. 
       Con la trasposizione questa è finita invece all'ultima riga, ma esclutendo proprio una componente e considerando il 3x3, la taglio fuori in ogni caso.
       Di seguito quindi la formula completa.
    */

    vert_world_norm_out = mat3(transpose(inverse(model))) * vert_normal; 
}
