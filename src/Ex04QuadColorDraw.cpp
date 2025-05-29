#include "Ex04QuadColorDraw.h"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include "OGLProgram.h"

struct Color
{
    float R;
    float G;
    float B;
    float A;

    Color(float inR, float inG, float inB, float inA) 
    : R(inR), G(inG), B(inB), A(inA)
    {}
};

Ex04QuadColorDraw::Ex04QuadColorDraw()
{
    Program = new OGLProgram("resources/shaders/quadcolor.vert", "resources/shaders/quadcolor.frag");

        // Posso aggiungere vicino ad ogni vertice il suo colore (RGBA), ricordo che in realtà è una sequenza di float contigua per la GPU, ed indichiam questa cosa con il link (punto 3)
        std::vector<float> Vertices = {
        // Triangle-right    // Colors
        -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,     // Bottom-left.
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,     // Bottom-right.
        0.5f, 0.5f, 0.0f,    0.0f, 0.0f, 1.0f,     // Top-right.

        // Triangle-left
        -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,     // Bottom-left.
        0.5f, 0.5f, 0.0f,    1.0f, 1.0f, 0.0f,     // Top-right.
        -0.5f, 0.5f, 0.0f,   0.0f, 1.0f, 0.0f      // Top-left.
    };

    // Dobbiamo passare i dati del triangolo alla GPU.

    // 1: Create VAO. "Portachiavi" di riferimento che si riferisce e controlla più buffer contemporaneamente.
    glGenVertexArrays(1, &Vao); //Creiamolo.
    glBindVertexArray(Vao); //Una volta bindato, tutti i buffer che creeremo si riferiranno al VAO attualmente bindato.

    // 2: Create VBO. Usato per caricare i dati dei vertici.
    glGenBuffers(1, &Vbo); // Creiamo il buffer.
    glBindBuffer(GL_ARRAY_BUFFER, Vbo); // In base a dove lo bindiamo ed il tipo, la GPU fa operazioni diverse. In questa maniera il VBO è legato al VAO bindato.
    // Se non bindassimo questo non sarebbe quello corrente preso in considerazione, quindi rischieremmo di caricare dati su un buffer precedente che non c'entra nulla con questa esecuzione su GPU.

    // Indichiamo alla GPU quanto questo buffer è grande.
    size_t DataSize = Vertices.size() *sizeof(float);
    glBufferData(GL_ARRAY_BUFFER, DataSize, Vertices.data(), GL_STATIC_DRAW); // Indichiamo alla GPU il buffer ed anche come useremo il buffer. GL_STATIC_DRAW indica che lo leggeremo spesso ma non verrà aggiornato.

    // 3: Link Buffer to Vertex Shader. Per il colore, prendo una nuova oposizione di vertici ogni 6 float, non ogni 3.
    GLuint Location_0 = 0;
    glVertexAttribPointer(Location_0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(Location_0);

    // Leggo i dati per il nuovo pin del colore. Legge ogni 6 con un offset iniziale di 3 float.
    GLuint Location_1 = 1;
    glVertexAttribPointer(Location_1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(Location_1);

    // 4: Set Viewport.
    glViewport(0, 0, 640, 460); // OpenGL parte da in basso a dx come origine.
    glClearColor(0.5f, 0.5f, 0.5f, 1.f);

    Program->Bind();

    //GLint BaseColorLocation = glGetUniformLocation(Program->ProgramId, "base_color");
}

Ex04QuadColorDraw::~Ex04QuadColorDraw()
{
    glDeleteVertexArrays(1, &Vao);
    glDeleteVertexArrays(1, &Vbo);
    delete Program;
}

void Ex04QuadColorDraw::Update(float DeltaTime)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6); // Prendiamo in considerazione 6 vertici totali, dallo 0.

}
