#include "Ex08PhongDraw.h"
#include <string>
#include <fstream>
#include <iostream>
#include "OGLProgram.h"
#include "OGLTexture.h"
#include "XCommon.h"
#include "obj_parser.h"

// Matrici per calcoli.
#include <glm/ext.hpp>


// Vogliamo riprodurre il Phong con l'obj dello storm-trooper.

Ex08PhongDraw::Ex08PhongDraw()
{
    Program = new OGLProgram("resources/shaders/phong.vert", "resources/shaders/phong.frag");

    // Chiaramente non indicheremo più i vertici a mano per un obj complesso, usiamo un parser.
    Obj obj;
    ObjParser::TryParseObj("resources/models/stormtrooper.obj", obj);

    // Da questo obj dobbiamo "srotolare" i vari valori per le posizioni, UVs, normali.
    for(size_t i = 0; i < obj.triangles.size(); ++i)
    {
        auto& t = obj.triangles[i];

        // ---------- Vertice 1 ----------

        // Posizioni.
        Vertices.push_back(t.v1.point.x);
        Vertices.push_back(t.v1.point.y);
        Vertices.push_back(t.v1.point.z);

        // UVs.
        Vertices.push_back(t.v1.uv.x);
        Vertices.push_back(t.v1.uv.y);

        // Normali.
        Vertices.push_back(t.v1.normal.x);
        Vertices.push_back(t.v1.normal.y);
        Vertices.push_back(t.v1.normal.z);


        // ---------- Vertice 2 ----------

        // Posizioni.
        Vertices.push_back(t.v2.point.x);
        Vertices.push_back(t.v2.point.y);
        Vertices.push_back(t.v2.point.z);

        // UVs.
        Vertices.push_back(t.v2.uv.x);
        Vertices.push_back(t.v2.uv.y);

        // Normali.
        Vertices.push_back(t.v2.normal.x);
        Vertices.push_back(t.v2.normal.y);
        Vertices.push_back(t.v2.normal.z);


        // ---------- Vertice 3 ----------

        // Posizioni.
        Vertices.push_back(t.v3.point.x);
        Vertices.push_back(t.v3.point.y);
        Vertices.push_back(t.v3.point.z);

        // UVs.
        Vertices.push_back(t.v3.uv.x);
        Vertices.push_back(t.v3.uv.y);

        // Normali.
        Vertices.push_back(t.v3.normal.x);
        Vertices.push_back(t.v3.normal.y);
        Vertices.push_back(t.v3.normal.z);
    }


    // 1: VAO.
    glGenVertexArrays(1, &Vao);
    glBindVertexArray(Vao);

    // 2: VBO.
    glGenBuffers(1, &Vbo);
    glBindBuffer(GL_ARRAY_BUFFER, Vbo);

    // La DataSize continua ad andare bene essendo dinamica.
    size_t DataSize = Vertices.size() * sizeof(float);
    glBufferData(GL_ARRAY_BUFFER, DataSize, Vertices.data(), GL_STATIC_DRAW); 

    // 3: Link del Buffer al Vertex Shader. 
    // Dobbiamo leggere correttamente dall'array, tenendo in considerazione delle UVs e normali. 
    // Le posizioni vengono prese di 8 in 8 partendo da 0, leggendo 3 alla volta.
    GLuint Location_0 = 0;
    glVertexAttribPointer(Location_0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(Location_0);

    // Leggo i dati per il pin UV.
    // Prendo le UV di 8 in 8, partendo dal terzo, leggendo 2 alla volta.
    GLuint Location_1 = 1;
    glVertexAttribPointer(Location_1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(Location_1);

    // Leggo i dati per il pin normal.
    // Prendo le UV di 8 in 8, partendo dal quinto, leggendo 3 alla volta.
    GLuint Location_2 = 2;
    glVertexAttribPointer(Location_2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(Location_2);

    // 4: Viewport.
    glViewport(0, 0, 640, 460);
    glClearColor(0.5f, 0.5f, 0.5f, 1.f);


    Program->Bind();


    // 5: Creiamo le texture. 
    StormTexture = new OGLTexture("resources/models/stormtrooper.png", true);

    // Rendiamo attuale la texture creata.
    StormTexture->Bind(GL_TEXTURE0);

    // 6: Enable Depth Testing.
    glEnable(GL_DEPTH_TEST);

    // 7: Enable Face Culling.
    glEnable(GL_CULL_FACE);

    // 8: Informazioni sulla camera. Sono i dati che servono a glm per eseguire tutti i calcoli.
    glm::vec3 Position = glm::vec3(0, 0, 8);
    glm::vec3 Direction = glm::vec3(0, 0, -1);
    glm::vec3 Up = glm::vec3(0, 1, 0); // Vettore up senza ulteriori rotazioni.
    float FovY = 60.f;
    float AspectRatio = 800.f / 600.f;
    float ZNear = 0.01f;
    float ZFar = 100.f;

    // Con GLM creiamo le varie matrici per i vari space studiati.

    // Creo ViewMatrix (matrice di view della camera). Il "mirino" della camera.
    View = glm::lookAt(Position, Position + Direction, Up);

    // Creo la matrice della projection.
    Projection = glm::perspective(glm::radians(FovY), AspectRatio, ZNear, ZFar);

    // PointLight per il phong.
    glm::vec3 PointLightPos = glm::vec3(4.f, 0, 0);
    Program->SetUniform("point_light_pos", PointLightPos);
}

Ex08PhongDraw::~Ex08PhongDraw()
{
    glDeleteVertexArrays(1, &Vao);
    glDeleteVertexArrays(1, &Vbo);

    delete StormTexture;
    delete Program;
}

void Ex08PhongDraw::Update(float DeltaTime)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Rotazione dinamica.
    static float ElapsedTime = 0;
    ElapsedTime += DeltaTime;
    float Angle = 20.f * ElapsedTime;


    // Rinnovo la matrice del modello, poichè volendolo ruotare cambierà nel tempo.
    // Ci sono due modi per fare questo. Vediamo il primo.

    // 1) ----------------------------------------------------------------------------------------------------------

    // Prendo una matrice identità.
    glm::mat4 Model = glm::mat4(1.f);

    // Applico le trasformazioni volute. 
    // Ricordo che queste verranno applicate al contrario per proprietà di matrici, quindi partirà dallo scale per l'effettiva applicazione.
    Model = glm::translate(Model, glm::vec3(0, -4, 0));
    Model = glm::rotate(Model, glm::radians(Angle), glm::vec3(0, 1, 0));
    Model = glm::scale(Model, glm::vec3(2.f));

    // Ottengo la matrice MVP (Model View Projection). Questo è effettivamente l'ordine voluto, quindi le devo concatenare al contrario.
    // La concatenazione tra matrici avviene col prodotto, e appunto deve essere fatta al contrario.
    glm::mat4 Mvp = Projection * View * Model;

    // Adesso la matrice deve essere mandata allo shader, e li avverrà l'aggiornamento.
    // Creiamo l'overload che accetta il tipo mat4 di glm.
    Program->SetUniform("mvp", Mvp);

    // Mando la matrice del modello per avere le normali e posizione i world position per creare il phong nello shader.
    Program->SetUniform("model", Model);

    // Prendiamo tutti i vertici dell'obj.
    glDrawArrays(GL_TRIANGLES, 0, Vertices.size());
}
