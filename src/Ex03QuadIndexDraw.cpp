#include "Ex03QuadIndexDraw.h"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include "OGLProgram.h"

Ex03QuadIndexDraw::Ex03QuadIndexDraw()
{
    Program = new OGLProgram("resources/shaders/triangle.vert", "resources/shaders/triangle.frag");

    // RIUSO GLI INDICI DEI VERTICI PRECEDENTI PER INDICARE I VERTICI UNICI.
    //     std::vector<float> Vertices = {
    //     // Triangle-right
    //     -0.5f, -0.5f, 0.0f, // Bottom-left.
    //     0.5f, -0.5f, 0.0f, // Bottom-right.
    //     0.5f, 0.5f, 0.0f, // Top-right.

    //     // Triangle-left
    //     -0.5f, -0.5f, 0.0f, // Bottom-left.
    //     0.5f, 0.5f, 0.0f, // Top-right.
    //     -0.5f, 0.5f, 0.0f // Top-left.
    // };

    // Adesso invece di due triangoli possiamo disegnare un quad. Evitiamo duplicazione di due vertici.
    // Dobbiamo indicare a GPU come interpretare quesrti 12 float.
    std::vector<float> Vertices = {
        // Triangle-right
        -0.5f, -0.5f, 0.0f, // Bottom-left.
        0.5f, -0.5f, 0.0f, // Bottom-right.
        0.5f, 0.5f, 0.0f, // Top-right.
        -0.5f, 0.5f, 0.0f // Top-left.
    };

    // Dobbiamo creare un buffer aggiuntivo con gli indici dei vertici che formano i triangoli.
    std::vector<uint32_t> Indexes = {
        0, 1, 2, // Triangle-Right.
        0, 2, 3 // Triangle Left.
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

    // 3: Link Buffer to Vertex Shader.
    GLuint Location_0 = 0;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(Location_0);

    // 4: Create EBO: Eleemnt Buffer Object.
    glGenBuffers(1, &Ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indexes.size() * sizeof(uint32_t), Indexes.data(), GL_STATIC_DRAW);

    // 5: Set Viewport.
    glViewport(0, 0, 640, 460); // OpenGL parte da in basso a dx come origine.
    glClearColor(0.5f, 0.5f, 0.5f, 1.f);

    Program->Bind();
}

Ex03QuadIndexDraw::~Ex03QuadIndexDraw()
{
    glDeleteVertexArrays(1, &Vao);
    glDeleteVertexArrays(1, &Vbo);
    delete Program;
}

void Ex03QuadIndexDraw::Update(float DeltaTime)
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Drawcall differente.
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

}
