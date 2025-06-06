#version 460 core

out vec4 frag_color;

// Creiamo pin di ingresso legato dal nome identificativo scelto nel .vert. La GPU mappa out->in sul nome scelto.
in vec4 vert_col_out;

// Dichiaro direttamente nel .frag, essendo uniform, e potremmo accedere a questa var ovunque nel programma.
uniform vec4 base_color;

void main()
{
    // Essendo un vec4 già in ingresso, possiamo direttamente assegnare al frag_color.
    // Adesso effettivamente, l'operazione avviene sui pixel come negli esercizi precedenti, ma questa volta il dato arriva dal programma.
    // frag_color = vert_col_out;

    // Mischiando i colori con la moltiplicazione, ed essendo valori tra 0 ed 1, otteremo valori più piccoli, quindi il tutto risulterà attenuato.
    // Abbiamo quindi la possibilità di usare operazioni matematiche per tipi che le permettono.
    frag_color = base_color * vert_col_out;
}