#version 460 core

layout (location = 0) in vec3 vert_pos;
layout (location = 1) in vec2 vert_uv;

out vec2 vert_uv_out;

// Funzione per calcolare la prospettiva.
vec3 perspective(vec3 pos)
{
    float aspect_ratio = 800.f / 600.f;
    float vertical_fov = 60.f; // L'apertura totale dell'occhio.

    // Dobbiamo calcolare la tangente. Ci serve metà angolo del FOV verticale, 
    // trasformati in radianti (funzione radians) per essere compatibili con le operazioni goniometriche.
    float fov =  tan(radians(vertical_fov * 0.5f));  

    // Ottengo quindi il fov reale da cui possiamo calcolare la prospettiva.
    vec3 persp_result;
    persp_result.x = pos.x / (fov * aspect_ratio);
    persp_result.y = pos.y / fov;
    persp_result.z = pos.z;

    // Entrambe le coordinate vanno quindi fixate per l'angolo di visione, ma solo una per l'aspect ratio, dipendentemente da come lo calcoliamo.
    // Lo "zoom" che vedremo nell'immagine è dovuto al FOV della camera applicato. 
    // Il fatto che adesso il box_texture sia tornato quadrato nonostante la finestra rettangolare, è dovuto al calcolo dell'Aspect Ratio.

    return persp_result;
}

void main()
{
    // Aggiusto la prospettiva. I vari parametri potrebbero essere passati come uniform, per ora i calcoli vanno bene qua.
    // Dobbiamo aggiustare quindi per l'Aspect Ratio, e poi aggiungere il FOV per la prospettiva.
    // Invece di farli però nel main dello shader, abbiamo la possibilità di creare altre funzioni.

    vec3 fixed_vert = perspective(vert_pos);

    gl_Position = vec4(fixed_vert, 1.f);
    vert_uv_out = vert_uv;
}
