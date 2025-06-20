#include "Ex08PhongDraw.h"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include "OGLProgram.h"
#include "OGLTexture.h"
#include "XCommon.h"

// Vogliamo disegnare un cubo, ed applicare correttamente la texture su tutte le faccie. 
// Teniamo conto di UV, prospettiva, AspectRatio e FOV.

Ex08PhongDraw::Ex08PhongDraw()
{
    Program = new OGLProgram("resources/shaders/cubepersp.vert", "resources/shaders/cubepersp.frag");

    // Per rappresentare un cubo, dobbiamo in qualche modo considerare tutte le 6 facce per i vari vertici.
    // Inoltre la Z non è più ignorabile.


    // Per chiarezza ometto il .f nei valori, tanto la costante accetta la conversione.
    // Le UV rimangono uguali poichè vogliamo la stessa texture uguale per ogni faccia.
    std::vector<float> Vertices = {
        
        // POSITION       // UVs

        // Front-face.    
        -1, -1,  1,       0, 0,     // Bottom-left.
         1, -1,  1,       1, 0,     // Bottom-right.
         1,  1,  1,       1, 1,     // Top-right.

        -1,  1,  1,       0, 1,     // Top-left.
        -1, -1,  1,       0, 0,     // Bottom-left.
         1,  1,  1,       1, 1,     // Top-right.

         
        // Back-face (definita però come se la stessimo guardando frontalmente).
         1, -1, -1,       0, 0,     // Bottom-left.
        -1, -1, -1,       1, 0,     // Bottom-right.
        -1,  1, -1,       1, 1,     // Top-right.

         1,  1, -1,       0, 1,     // Top-left.
         1, -1, -1,       0, 0,     // Bottom-left.
        -1,  1, -1,       1, 1,     // Top-right.


        // Left-face.  
        -1, -1, -1,       0, 0,    // Bottom-left.
        -1, -1,  1,       1, 0,    // Bottom-right.
        -1,  1,  1,       1, 1,    // Top-right.

        -1,  1, -1,       0, 1,    // Top-left.
        -1, -1, -1,       0, 0,    // Bottom-left.
        -1,  1,  1,       1, 1,    // Top-right.
  

        // Right-face.        
         1, -1,  1,       0, 0,    // Bottom-left.
         1, -1, -1,       1, 0,    // Bottom-right.
         1,  1, -1,       1, 1,    // Top-right.

         1,  1,  1,       0, 1,    // Top-left.
         1, -1,  1,       0, 0,    // Bottom-left.
         1,  1, -1,       1, 1,    // Top-right.
  

        // Top-face.        
        -1,  1,  1,       0, 0,    // Bottom-left.
         1,  1,  1,       1, 0,    // Bottom-right.
         1,  1, -1,       1, 1,    // Top-right.

        -1,  1, -1,       0, 1,    // Top-left.
        -1,  1,  1,       0, 0,    // Bottom-left.
         1,  1, -1,       1, 1,    // Top-right.
           

        // Bottom-face.  
        -1, -1, -1,       0, 0,    // Bottom-left.
         1, -1, -1,       1, 0,    // Bottom-right.
         1, -1,  1,       1, 1,    // Top-right.

        -1, -1,  1,       0, 1,    // Top-left.
        -1, -1, -1,       0, 0,    // Bottom-left.
         1, -1,  1,       1, 1,    // Top-right.
  
    };


    // Dobbiamo passare i dati del cubo alla GPU.

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


    // 5: Creiamo le immagini. 
    // Attenzione che le texture, quando non le usiamo più vanno distrutte.
    BoxTexture = new OGLTexture("resources/textures/wood-box.jpg", true);

    // Rendiamo attuale la texture creata.
    BoxTexture->Bind(GL_TEXTURE0);

    // 6: Enable Depth Testing.
    // Serve lo Z buffer per far funzionare la profondità, va attivato in maniera esplicita.
    glEnable(GL_DEPTH_TEST);

    // 7: Enable Face Culling.
    // Anche il face-culling deve essere abilitato in maniera esplicita, 
    // inoltre è possibile anche specificarne la modalità del cull (quale faccia tagliare).
    // È possibile anche decidere quale è la front-face secondo vertici orari o antiorari (di default è questa).
    glEnable(GL_CULL_FACE);

    // glCullFace(GL_BACK); // Default.
    // glFrontFace(GL_CCW); // Default.
}

Ex08PhongDraw::~Ex08PhongDraw()
{
    glDeleteVertexArrays(1, &Vao);
    glDeleteVertexArrays(1, &Vbo);

    delete BoxTexture;
    delete Program;
}

void Ex08PhongDraw::Update(float DeltaTime)
{
    // Una volta abilitato il depth-buffer, dobbiamo pure pulirlo.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Rotazione dinamica cubo.
    static float ElapsedTIme = 0;
    ElapsedTIme += DeltaTime;
    Program->SetUniform("angle", 20 * ElapsedTIme);

    // Dobbiamo renderizzare tutti i 36 vertici, non solo la faccia davanti.
    glDrawArrays(GL_TRIANGLES, 0, 36);
}
