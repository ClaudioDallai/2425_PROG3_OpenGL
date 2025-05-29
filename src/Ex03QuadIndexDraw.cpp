#include "Ex03QuadIndexDraw.h"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include "OGLProgram.h"

Ex03QuadIndexDraw::Ex03QuadIndexDraw()
{
    // Questa volta creiamo un programma sfruttando una nostre classe custom che wrappa le funzioni che abbiamo usato negli esercizi precedenti.
    // In questo modo ci facciamo tornare direttamente il programma. Ricordiamoci sempre di distruggere la memoria allocata con new.
    Program = new OGLProgram("resources/shaders/triangle.vert", "resources/shaders/triangle.frag");

    // Riutilizzo le posizioni NDC dei vertici nell'esercizio 02 per indicare i nuovi 4 vertici unici del quad.
    // Adesso infatti, invece di due triangoli possiamo disegnare un quad. Evitiamo la duplicazione di due vertici.
    // Dobbiamo indicare alla GPU come interpretare questi 12 float.

    // Si adopera questa modalità per ridurre il numero di dati di vertici da mandare alla GPU. 
    // È ciò che si usa effettivamente per le mesh. Va capito però sempre se è conveniente o meno, poichè abbiamo bisogno di un buffer aggiuntivo.
    // Questo principio si applica anche a tutti gli altri buffer, come UV, normali...

    std::vector<float> Vertices = {
        // Quad:
        -0.5f, -0.5f, 0.0f, // Bottom-left.
        0.5f, -0.5f, 0.0f,  // Bottom-right.
        0.5f, 0.5f, 0.0f,   // Top-right.
        -0.5f, 0.5f, 0.0f   // Top-left.
    };

    // Dobbiamo creare un buffer aggiuntivo con gli indici dei vertici che formano i triangoli per la costruzione del quad.
    // Questo serve per l'interpretazione da parte della GPU. È un livello di indirezione.
    std::vector<uint32_t> Indexes = {
        0, 1, 2,  // Triangle-Right.
        0, 2, 3   // Triangle Left.
    };

    // Dobbiamo passare i dati dei triangoli alla GPU.

    // 1: VAO.
    glGenVertexArrays(1, &Vao);
    glBindVertexArray(Vao);


    // 2: VBO.
    glGenBuffers(1, &Vbo);
    glBindBuffer(GL_ARRAY_BUFFER, Vbo);

    // Grandezza buffer, anche con questi dati in più, la dimensione così passata rimane corretta.
    size_t DataSize = Vertices.size() * sizeof(float);
    glBufferData(GL_ARRAY_BUFFER, DataSize, Vertices.data(), GL_STATIC_DRAW);


    // 3: Link del buffer al Vertex Shader. Poichè disegniamo sempre e solo triangoli, questi parametri rimangono invariati.
    GLuint Location_0 = 0;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(Location_0);


    // 4: Creo un buffer EBO: Element Buffer Object.
    // Questo è un passaggio aggiuntivo per far conoscere alla GPU il buffer usato per l'interpretazione dei vertici di triangoli, attraverso gli indici.
    glGenBuffers(1, &Ebo); // Creiamo il buffer EBO.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Ebo); // Bindiamo il buffer, anche questo al VAO attualmente attivo.
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indexes.size() * sizeof(uint32_t), Indexes.data(), GL_STATIC_DRAW); // Specifichiamone grandezza, dati, e modalità per la GPU.


    // 5: Impostiamo la Viewport.
    glViewport(0, 0, 640, 460);
    glClearColor(0.5f, 0.5f, 0.5f, 1.f);

    Program->Bind(); // Bindiamo il programma. Sfruttiamo la funzione wrapper della nostra classe.
}

Ex03QuadIndexDraw::~Ex03QuadIndexDraw()
{
    glDeleteVertexArrays(1, &Vao);
    glDeleteBuffers(1, &Vbo);
    glDeleteBuffers(1, &Ebo); // Ricordiamoci sempre di distruggere i buffer creati.
    delete Program;
}

void Ex03QuadIndexDraw::Update(float DeltaTime)
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Usiamo una drawcall differente. Ci permette di disegnare elementi presi singolarmente. 
    // Quindi per 2 triangoli risultano 6 elementi della tipologia specificata.
    // Questa drawcall legge si il buffer dei dati VBO, ma lo indicizza come specificato dall'EBO. Si aspetta quindi che esista un EBO bindato.
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
}
