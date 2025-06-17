#version 460 core

layout (location = 0) in vec3 vert_pos;
layout (location = 1) in vec2 vert_uv;

out vec2 vert_uv_out;

// Funzione per calcolare la prospettiva.
// Per funzionare in 3D, anche la Z deve essere trasformata per riportare tutto in NDC (Z positiva che si allontana da noi, Left-handed).
// Deve infatti essere una prospettiva reale.
vec3 perspective(vec3 pos)
{
    float aspect_ratio = 800.f / 600.f;
    float vertical_fov = 60.f;

    float fov =  tan(radians(vertical_fov * 0.5f));  



    // La Z è negativa poichè se mi arrivasse un valore negativo, vedremmo l'asse ribaltato.
    vec3 persp_result;
    persp_result.x = pos.x / -pos.z;
    persp_result.y = pos.y / -pos.z;




    persp_result.x = pos.x / (fov * aspect_ratio);
    persp_result.y = pos.y / fov;
    persp_result.z = pos.z;

    return persp_result;
}

void main()
{
    // Prima della prospettiva, poichè non siamo in NDC ma è come se fossimo in coordinate mondo, 
    // prima trasformiamo il modello, poi lo passiamo alla prospettiva.

    vec3 fixed_vert = vert_pos;

    // Calcoliamo quindi la traslazione in modo tale da vedere il cubo.
    // Essendo Left-Handed dobbiamo aggiungere sulla Z (per allontanarci) un numero di unità (abbastanza arbitrario).
    fixed_vert.z += 4.f;

    // Prospettiva 3D.
    fixed_vert = perspective(fixed_vert);

    gl_Position = vec4(fixed_vert, 1.f);
    vert_uv_out = vert_uv;
}
