#version 460 core

out vec4 frag_color;

// Mi lego in input al pin di uscita del .vert.
in vec2 vert_uv_out;

// Il sampler2D è una tipologia di var usata per le texture2D. Contiene lo slot attivato della texture (quindi lo 0).
// Essendo un sampler, la GPU viene re-indirizzata nello slot0 attivo e legge i dati che contiene.
// uniform sampler2D smile_tex;

// Un altro modo per selezionare la uniform ed associarla allo slot attivo (0 in questo caso), è usare una direttiva di binding.
// Applichiamo un Layout alla uniform con specifica di binding allo slot voluto.
layout (binding = 0) uniform sampler2D smile_tex;

// Esempio per texture aggiuntiva. Ci bindiamo questa volta allo slot 1.
layout (binding = 1) uniform sampler2D box_tex;

void main()
{
    // Campioniamo il colore dalla texture, sfruttando l'identificatore (il puntatore) allo slot.
    // Per capire quale pixel campionare, ci servono le UV. È una informazione per vertice.

    // frag_color = texture(smile_tex, vert_uv_out);
    // frag_color = texture(box_tex, vert_uv_out);

    // Possiamo anche mischiare insieme due texture.
    vec4 smile_texel = texture(smile_tex, vert_uv_out);
    vec4 box_texel = texture(box_tex, vert_uv_out);

    // Usiamo la funzione di blend chiamata mix.
    frag_color = mix(smile_texel, box_texel, 0.5f);
    
}