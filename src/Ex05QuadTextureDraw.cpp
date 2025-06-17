#include "Ex05QuadTextureDraw.h"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include "OGLProgram.h"
#include "OGLTexture.h"
#include "XCommon.h"


// Vogliamo leggere una texture.
// Per leggere una immagine sfruttiamo stb_image con la API di load. 
// Una volta ottenuto il buffer, dobbiamo creare una etxture su GPU e caricarci i dati che abbiamo ottenuto.


Ex05QuadTextureDraw::Ex05QuadTextureDraw()
{
    // Usiamo nuovi shader che gestiscono texture, ovvero hanno informazioni e calcoli anche sulle UV.
    Program = new OGLProgram("resources/shaders/quadtexture.vert", "resources/shaders/quadtexture.frag");

    // Invece dei colori, gestiamo le UVs.
    std::vector<float> Vertices = {
        // Triangle-right    // UVs
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,     // Bottom-left.
        0.5f, -0.5f, 0.0f,   1.0f, 0.0f,     // Bottom-right.
        0.5f, 0.5f, 0.0f,    1.0f, 1.0f,     // Top-right.
        // Triangl
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,     // Bottom-left.
        0.5f, 0.5f, 0.0f,    1.0f, 1.0f,     // Top-right.
        -0.5f, 0.5f, 0.0f,   0.0f, 1.0f      // Top-left.
    };


    // Dobbiamo passare i dati del triangolo alla GPU.

    // 1: VAO.
    glGenVertexArrays(1, &Vao);
    glBindVertexArray(Vao);

    // 2: VBO.
    glGenBuffers(1, &Vbo);
    glBindBuffer(GL_ARRAY_BUFFER, Vbo);

    // Indichiamo alla GPU quanto questo buffer è grande.
    size_t DataSize = Vertices.size() *sizeof(float);
    glBufferData(GL_ARRAY_BUFFER, DataSize, Vertices.data(), GL_STATIC_DRAW); 

    // 3: Link del Buffer al Vertex Shader. 
    // Dobbiamo leggere correttamente dall'array, tenendo in considerazione delle UVs. Le posizioni vengono prese di 5 in 5 partendo da 0.
    GLuint Location_0 = 0;
    glVertexAttribPointer(Location_0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(Location_0);

    // Leggo i dati per il nuovo pin UV.
    // Prendo le UV di 5 in 5, partendo dal terzo, leggendo due alla volta.
    GLuint Location_1 = 1;
    glVertexAttribPointer(Location_1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(Location_1);

    // 4: Viewport.
    glViewport(0, 0, 640, 460);
    glClearColor(0.5f, 0.5f, 0.5f, 1.f);


    Program->Bind();


    // Texture.

    // 5: Creiamo le immagini. 
    // Attenzione che le texture, quando non le usiamo più vanno distrutte.
    SmileTexture = new OGLTexture("resources/textures/smile.png", true);
    BoxTexture = new OGLTexture("resources/textures/wood-box.jpg", true);

    // Rendiamo attuale la texture creata.
    // Va abilitato lo slot della texture. La GPU ha la texture caricata in memoria, ma va impostata attiva per gli slot degli shader.
    // Gli slot vanno attivati in maniera ordinata, partendo dallo 0.
    // Potrebbe non essere necessario poichè la bindiamo nel CreateTexture, ma ripetere l'operazione aggiorna in maniera corretta la macchina a stati, evitando problemi.
    SmileTexture->Bind(GL_TEXTURE0);


    // La texture è costante per ogni vertice. 
    // Quindi può essere rappresentata con una uniform. Possiamo crearla direttamente dove serve, quindi nel .frag in questo caso.
    // Il valore 0 nella funzione indica lo slot che abbiamo attivato. Quindi assegniamo il valore 0 alla uniform. In modo da attaccare il Sampler2D a quello slot.
    // glUniform1i(glGetUniformLocation(Program->GetProgramID(), "smile_tex"), 0); 

    // ATTENZIONE: Alternativamente questo bind ella uniform allo slot può essere effettuato nello shader con la direttiva di binding.
    // È ciò che avviene in questo esempio, e quindi quello scritto sopra non è più necessario. Possiamo usare una delle due modalità.
    // Il funzionamento è quindi spostato totalmente nel .vert.


    // Lavoriamo adesso anche su un'altra immagine creata in contemporanea alla prima.
    // Questa volta ci dobbiamo collegare allo slot 1.
    BoxTexture->Bind(GL_TEXTURE1);
    
    // Nello shader fragment, possiamo anche giocare con queste texture, per esempio interpolandole tra loro.
    // Per controllare la trasparenza, quindi l'Alpha-Blending, dobbiamo attivarla in maniera esplicita.


    // 6: Attivare AlphaBlending.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Specifichiamo la funzione di blend che vogliamo usare. 1-X è un AlphaBlending classico.
}

Ex05QuadTextureDraw::~Ex05QuadTextureDraw()
{
    glDeleteVertexArrays(1, &Vao);
    glDeleteVertexArrays(1, &Vbo);

    // Essendo adesso OGLTexture un oggetto assestante, istanziato con new, dobbiamo liberarne la memoria.
    delete SmileTexture;
    delete BoxTexture;

    delete Program;
}

void Ex05QuadTextureDraw::Update(float DeltaTime)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
