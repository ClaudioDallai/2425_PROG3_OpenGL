#include "Ex04QuadColorDraw.h"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include "OGLProgram.h"

// Vogliamo in questo esercizio far in modo di riuscire ad immettere un colore diverso per ogni vertice.

// Vogliamo anche poter cambiare la dimensione del quad, ovvero scalare i vertici senza cambiare buffer e ricaricarlo.
// Sulla stessa mesh fissa quindi, con la GPU nel vertex shader, possiamo agire sui vertici. È una uniform creata nel .vert (sotto spiegato cosa è).

// Serve una struttura per immagazzinare il colore per ogni Pixel.
struct Color
{
    float R;
    float G;
    float B;
    float A;
};

Ex04QuadColorDraw::Ex04QuadColorDraw()
{
    // Usiamo degli shader diversi dagli altri esercizi, così da non comprometterli. Alla fine gli shader non sono altro che assets.
    Program = new OGLProgram("resources/shaders/quadcolor.vert", "resources/shaders/quadcolor.frag");

    // Posso aggiungere vicino ad ogni vertice il suo colore (RGBA), ricordo che in realtà è una sequenza di float contigua per la GPU, 
    // ed indichiamo alla GPU questa cosa (la suddivisione dei vertici tra la lista dei float) con il link (punto 3).
    std::vector<float> Vertices = {
        // Triangle-right    // Colors
        -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,     // Bottom-left.
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,     // Bottom-right.
        0.5f, 0.5f, 0.0f,    0.0f, 0.0f, 1.0f,     // Top-right.

        // Triangle-left
        -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,     // Bottom-left.
        0.5f, 0.5f, 0.0f,    0.0f, 0.0f, 1.0f,     // Top-right.
        -0.5f, 0.5f, 0.0f,   1.0f, 1.0f, 0.0f      // Top-left.
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
    // Per il colore, devo prendere una nuova posizione di vertici ogni 6 float, non ogni 3 come negli esercizi precedenti.
    GLuint Location_0 = 0;
    glVertexAttribPointer(Location_0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(Location_0);

    // Leggo i dati per il nuovo pin del colore nella locazione corretta. La GPU leggerà ogni 6 con un offset di 3 float.
    GLuint Location_1 = 1;
    glVertexAttribPointer(Location_1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(Location_1);



    // 4: Viewport.
    glViewport(0, 0, 640, 460);
    glClearColor(0.5f, 0.5f, 0.5f, 1.f);


    Program->Bind();


    // Possiamo anche inserire un colore di base per la figura, a cui poi verrà applicato sopra il colore dei vari vertici, poi pixel, nella rasterizzazione.
    // Queste variabili "fisse", che identificano un dato costante durante la drawcall, vengono chiamate: uniform.
    // Queste var uniform, non devono essere passate dal programma iniziale vertex al frag, 
    // ma essendo delle costanti possono essere accedute da qualsiasi shader facente parte di quel programma.

    // Accedo quindi alla uniform creata direttamente nel .frag, mischiando i colori per esempio con quelli dati dal vertice (vedi quadcolor.frag).
    // Accedo ad una var uniform attraverso il suo nome, shelto nello shader dove è stata creata. Devo anche indicare l'ID del programma a cui fa riferimento.
    
    // Definire le uniform assegna loro una sorta di position. La posizione che prendono dipende dall'ordine in cui sono state definite.
    // Con la funzione sottostante prendiamo quindi la locazione della uniform specificata, è come se ne prendessimo il puntatore (-1 se non la trova).
    GLint BaseColorLocation = glGetUniformLocation(Program->GetProgramID(), "base_color");

    // Prepariamo il colore, sfruttando una struct Color.
    Color BaseColor(0.5f, 0.5f, 0.5f, 1.f);

    // Impostiamo il colore. Scegliamo la API corrispondente per accedere al tipo corretto di uniform.
    // Nel nostro caso accediamo ad una uniform vec4, quindi 4 float variable (4fv). 
    // Anche questa API funziona a lista, indichiamo che vogliamo passare solo un vec4. Ed infine il puntatore ai 4 float, che otteniamo con un reinterpret_cast.
    // A livello di memoria, essendo che la struct inizia con un float e sono 4, possiamo considerarli come successione di float (array / float*).
    const GLfloat* BaseColorPtr = reinterpret_cast<GLfloat*> (&BaseColor);
    glUniform4fv(BaseColorLocation, 1, BaseColorPtr);

    // I vertici, essendo colorati su una base uniforme grigia che stiamo passando, risultano giustamente sbiaditi.

    // ATTENZIONE: Una uniform può anche essere mdoificata più volte durante l'esecuzione del Program. 
    // La cosa importante che la rende uniform è che sia costante nella drawcall, ma prima e dopo di essa è sempre modificabile.
    // Un esempio di questo, parametrizzato relativo al tempo, è presente nell'Update.

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

    // Esempio uniform modificata in relazione al tempo.
    static float ElapsedTime = 0;
    ElapsedTime += DeltaTime;

    // Usiamo sin e cos per randomizzare il valore, mantenendo l'intervallo costante (rimappato tra 0 ed 1, non tra -1 ed 1).
    Color TimedColor;
    TimedColor.R = sinf(ElapsedTime) * 0.5f + 0.5f; // [-1, 1] -> [0, 1]
    TimedColor.G = cosf(ElapsedTime) * 0.5f + 0.5f;
    TimedColor.B = cosf(ElapsedTime + 1.1f) * 0.5f + 0.5f;
    TimedColor.A = 1.f;

    // Assegno il valore alla uniform.
    GLint BaseColorLocation = glGetUniformLocation(Program->GetProgramID(), "base_color");
    glUniform4fv(BaseColorLocation, 1, reinterpret_cast<GLfloat*> (&TimedColor));


    // Esempio per scrivere dentro l'uniform scale, per agire sulla dimensione dei vertici.
    float scale_speed = 10.0f; // Moltiplicando l'angolo, l'oscillazione del sin/cos aumenta/diminuisce a seconda della moltiplicazione.
    float scale = sinf(ElapsedTime * scale_speed) * 0.5f + 0.5f; // 0% - 100%. Essendo uno scale potrei andare anche oltre il 100% della dimensione originale.
    glUniform1f(glGetUniformLocation(Program->GetProgramID(), "scale"), scale); // Se lo scale diventasse negativo, i vertici vengono invertiti.


    glDrawArrays(GL_TRIANGLES, 0, 6); // Prendiamo in considerazione 6 vertici totali, dallo 0.

}
