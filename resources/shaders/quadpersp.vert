#version 460 core

layout (location = 0) in vec3 vert_pos;

// Pin per le UV. Lo creiamo nel .vert per poterlo interpolare usando la GPU.
layout (location = 1) in vec2 vert_uv;

// Pin di output per le UV.
out vec2 vert_uv_out;

void main()
{
    gl_Position = vec4(vert_pos, 1.f);

    // Lo ripasso in output senza cambiamenti.
    vert_uv_out = vert_uv;

    // Flippiamo la Y, altrimenti per come funziona il mapping delle UV con lo standard gl, risulta capovolto.
    // Possiamo alternativamente anche sfruttare le API di stb in lettura.
    // vert_uv_out.y = 1.f - vert_uv_out.y;

    // Se per esempio l'operazione per aggiustare l'immagine fosse molto costosa, conviene farla sempre qua nel .vert.
    // Questo perchè nel vertex verrebbe effettuata una volta per vertice, nel fragment invece una volta per pixel, risultando troppo più pesante inutilmente.

    // Dipendentemente dall'effetto voluto però, per avere comportamenti desiderati senza interpolazioni della rasterizzazione, 
    // dobbiamo lavorare sul fragment, per avere proprio l'effetto 1 ad 1 sul pixel.
}
