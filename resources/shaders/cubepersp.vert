#version 460 core

layout (location = 0) in vec3 vert_pos;
layout (location = 1) in vec2 vert_uv;

uniform float angle;

out vec2 vert_uv_out;

// Funzione per calcolare la prospettiva.
// Per funzionare in 3D, anche la Z deve essere trasformata per riportare tutto in NDC (Z positiva che si allontana da noi, Left-handed).
// Deve infatti essere una prospettiva reale.
vec3 perspective(vec3 pos)
{
    float aspect_ratio = 800.f / 600.f;
    float vertical_fov = 60.f;

    float fov =  tan(radians(vertical_fov * 0.5f));  

    // Near e Far Plane.
    float near = 0.01f;
    float far = 100.f;

    float range = far - near;

    // La Z è negativa poichè se mi arrivasse un valore negativo, vedremmo l'asse ribaltato.
    vec3 persp_result;
    persp_result.x = pos.x / -pos.z;
    persp_result.y = pos.y / -pos.z;

    // Portiamo la Z nel range tra near e far. 
    // Trasformo il sistema di coordinate, la Z deve essere rimappata sul range, rimanendo tra -1 ed 1.
    // Quindi stiamo trasformando la Z da un Righ-hand-system (world), in un Left-hand-system (NDC). 
    // Di conseguenza devo anche invertire la Z, come citato sopra.

    persp_result.z = ((pos.z - near) / range) * 2.f + 1.f;
    persp_result.z *= -1.f;

    // Aggiustiamo col FOV. La Z è già stata calcolata.
    persp_result.x /= (fov * aspect_ratio);
    persp_result.y /= fov;

    return persp_result;
}

// Funzione per la rotazione di un vertice sul un piano 2D.
vec3 rotate_on_y(vec3 pos, float degrees)
{
    // Trasformo gradi in radianti.
    float rads = radians(degrees);

    vec3 result;

    // Formula per la rotazione. Se ruotiamo su Y, questo rimane invariato.
    result.x = cos(rads) * pos.x - sin(rads) * pos.z;
    result.y = pos.y;
    result.z = sin(rads) * pos.x + cos(rads) * pos.z;

    return result;
}

void main()
{
    // Prima della prospettiva, poichè non siamo in NDC ma è come se fossimo in coordinate mondo, 
    // prima trasformiamo il modello, poi lo passiamo alla prospettiva.

    vec3 fixed_vert = vert_pos;

    // Rotazione. Da fare PRIMA della traslazione: prima si ruota sul posto e poi lo trasliamo.
    fixed_vert = rotate_on_y(fixed_vert, angle);

    // Calcoliamo quindi la traslazione in modo tale da vedere il cubo.
    // Trasliamo tutti i punti sulla Z negativa, di un numero di unità (abbastanza arbitrario).
    fixed_vert.z -= 4.f;

    // Prospettiva 3D.
    fixed_vert = perspective(fixed_vert);

    // Il parametro 1.f hardcodato, è la causa della distorsione nella texture mentre il modello ruota.
    // Quando le UV vengono passate in uscita ed assegnate nel frag, la GPU inizia ad interpolare.
    // L'interpolazione va fixata per la prospettiva, e questo parametro si occupa di questo.
    // Questo quarto parametro applica la prospettiva corretta su ogni parametro uscente verso il frag, e per questo motivo andrà gestito.
    gl_Position = vec4(fixed_vert, 1.f);
    vert_uv_out = vert_uv;
}
