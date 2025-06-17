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


    // --------------------------------------------------------------- //

    // Esercizio per corretto calcolo Alpha. Non devo rendere in trasparenza l'immagine sottostante, se quella sopra ha una parte "trasparente".
    // RICORDA: vec3 full_transp = vec4(0, 0, 0, 0);
    // Esistono anche gli if per quanto pesi su un frag. Alla fine gli shader sono C senza puntatori.

    vec4 color;
    if (smile_texel.a > 0.f)
    {
        color = mix(smile_texel, box_texel, 0.5f);
    }
    else
    {
        color = box_texel;
    }

    // frag_color = color;

    /*
        ATTENZIONE - BRANCHING GPU: Le operazioni di branching possono provocare jump da un punto ad un altro, rallentando la CPU. 
        La CPU infatti, per capire quali dati tenersi in cache, cerca pure di predirre i branching. Se sbaglia è costretta a ricaricare i dati.
        L'operazione del branching, può dare problemi di performance anche in GPU, ma non per lo stesso motivo.
        Le GPU sono ottimizzate per conti in parallelo. La GPU per essere performante viene evitato che faccia switch vari, salti, e context switch.
        Risolve la questione facendo totalmente i calcoli. Esegue tutti e due i branch e salti, e SOLO alla fine scarta quello che non serve.
        Il problema di eprformance avviene quindi quando nei branching si eseguono molte operazioni, magari anche pese, 
        e praticamente uno dei due possibili salti è completamente inutile, ma viene comunque svolto sempre.

        Un caso come quello sopra, un mix ed una assegnazione, non provoca nessun problema di nessun tipo, essendo solo 2 operazioni leggere.
    */

    // Alternativamente lo stesso esercizio poteva essere fatto senza branching.
    // (1.f - 0.5f) indica la trasparenza che avevamo impostato negli esercizi precedenti. Questo verrà sottrato all'1 che indica mix massimo.
    // Non è lo stesso comportamento, ma il risultato è corretto.

    float factor = (1.f - (smile_texel.a * (1.f - 0.5f)));
    color = mix(smile_texel, box_texel, factor);
    // frag_color = color;


    // La vera risoluzione dell'esercizio, simile come ragionamento al metodo precedente, usa la funzione "Step".
    // La funzione step(VALUE, COMP) ha comportamento: 
    // - se COMP è superiore a VALUE allora viene ritornato 1, 
    // - se COMP è inferiore a VALUE viene ritornato 0.
    // - nel caso siano uguali, allora viene ritornato 1.

    // Quando alpha = 0, allora is_visible = 0. Quando alpha > 0, allora is_visible = 1.
    float is_visible = step(0.f, smile_texel.a);
    
    // --------------------------------------------------------------- //


    // Usiamo la funzione di blend chiamata mix.
    // frag_color = mix(smile_texel, box_texel, 0.5f);
}


// CURIOSITÀ:
// Esistono shortcut per poter eseguirec e quindi accedere, calcoli su più componenti insieme di un vec.
// Posso per esempio accedere a vec.rgba, vec.rgb, vec.rg, vec.r, vec.xyz.
// Ovviamente, essendo puramente posizionale ed usato solo per praticita, scrivere vec.x == vec.r. 
// Non sono parametri diversi, ma un accesso veloce allo slot, con nomenclature diverse utili al contesto.
