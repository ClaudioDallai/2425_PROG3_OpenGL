#include "Ex02QuadDraw.h"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

// Funzione per leggere file. Dovremmo leggere i file contenenti gli shader.
static std::string ReadFile(const std::string& InPath)
{
    // Ci serve caricare tutto il file in memoria come stringa, poichè dobbiamo compilarlo.
    // Lo shader ha bisogno della compilazione per essere usato.
    // Poichè dovremmo leggere tutto, per non dover concatenare i buffer di appoggio con la stringa da costruire,
    // dovremmo sapere quanto è grande il file. Potremmo allocare totalmente il buffer e leggere tutto insieme, un "ReadAll".
    // Possoiamo aprire il file, portare alla fine il cursore (apertura ate ovvero at the end), 
    // e posso chiedere allo stream quanto il cursore è stato scorso. Quello ``e il filesize.

    std::ifstream InputStream(InPath, std::ios::ate);
    size_t FileSize = InputStream.tellg();

    std::string Text;
    Text.resize(FileSize);

    // Dopo aver saputo quanto è grande il file per allocare il buffer, risposto il cursore all'inizio (beg ovvero begin).
    // Proseguo a leggere il file partendo dal primo byte (&Text[0]).
    InputStream.seekg(0, std::ios::beg);
    InputStream.read(&Text[0], FileSize);

    InputStream.close();
    return Text;
}

// Devo creare lo shader. Devo distinguere se è di tipo frag o vert.
static GLuint CreateShader(const std::string& InPath, GLuint InShaderType)
{
    std::string Text = ReadFile(InPath);
    const char* ShaderSource = Text.c_str(); // Puntatore al sorgente dello shader.

    // Creiamo lo Shader e carichiamo i sorgenti sulla GPU.
    GLuint ShaderId = glCreateShader(InShaderType);
    glShaderSource(ShaderId, 1, &ShaderSource, NULL);

    // Compiliamo.
    glCompileShader(ShaderId);

    // Essenbdo compilato su GPU, se fallisce non vedremo niente. Dobbiamo chiedere alla GPU se il tutto è riuscito.
    // Sfruttiamo API di richiesta informazioni.
    GLint Success;
    glGetShaderiv(ShaderId, GL_COMPILE_STATUS, &Success);

    // 0 -> falso; 1 -> true.
    if (!Success)
    {
        GLint MaxLogLength; // massimo log che la GPU può scrivere.
        glGetShaderiv(ShaderId, GL_INFO_LOG_LENGTH, &MaxLogLength);
        
        // Otteniamo un puntatore di char dal vettore MODIFICABILE, 
        // così possiamo scrivere i mess di errorre dall'inizio del puntatore fino alla fine della grabndezza MaxLogLength.
        std::vector<GLchar> InfoLog(MaxLogLength);
        glGetShaderInfoLog(ShaderId, MaxLogLength, NULL, InfoLog.data());

        // Stampiamo le informazioni a schermo. Costruiamo la stringa dal vettore, usando gli iteratori.
        std::string LogStr(InfoLog.begin(), InfoLog.end());
        std::cout << "[ERRO] Shader compilation failure: " << LogStr;
        throw std::runtime_error(LogStr);
    }

    return ShaderId;
}

static GLuint CreateProgram(GLuint VertexShaderId, GLuint FragmentShaderId)
{
    GLuint ProgramId = glCreateProgram();
    glAttachShader(ProgramId, VertexShaderId);
    glAttachShader(ProgramId, FragmentShaderId);
    glLinkProgram(ProgramId);

    // Stampiamo eventuali errori GPU, stessa logica vista.
    GLint Success;
    glGetProgramiv(ProgramId, GL_LINK_STATUS, &Success);
    if (!Success)
    {
        GLint MaxLogLength;
        glGetShaderiv(ProgramId, GL_INFO_LOG_LENGTH, &MaxLogLength);

        std::vector<GLchar> InfoLog(MaxLogLength);
        glGetProgramInfoLog(ProgramId, MaxLogLength, NULL, InfoLog.data());

        std::string LogStr(InfoLog.begin(), InfoLog.end());
        std::cout << "[ERRO] Shader Linking failure: " << LogStr;
        throw std::runtime_error(LogStr);

    }

    // Una volta creato il programma, le risorse degli shader possono essere cancellate.
    // Abbiamo creato il programma e compilato, le "librerie statice", come similutudine agli shader, non sono più necessari.
    glDeleteShader(VertexShaderId);
    glDeleteShader(FragmentShaderId);

    return ProgramId;
}

Ex02QuadDraw::Ex02QuadDraw()
{
    // L'estensione può essere qualsiasi, usaimo vert e frag per chiarezza.
    GLuint VertexShaderId = CreateShader("resources/shaders/triangle.vert", GL_VERTEX_SHADER);
    GLuint FragmentShaderId = CreateShader("resources/shaders/triangle.frag", GL_FRAGMENT_SHADER);

    ProgramId = CreateProgram(VertexShaderId, FragmentShaderId);

    // Counter-clock wise. By now in vertex data in NDC space.
    // Left-handed rule. But Z is 0 so is not important.

    // Per fare un quadrato, lo trattiamo come due triangoli. |/ /|
    std::vector<float> Vertices = {
        // Triangle-right
        -0.5f, -0.5f, 0.0f, // Bottom-left.
        0.5f, -0.5f, 0.0f, // Bottom-right.
        0.5f, 0.5f, 0.0f, // Top-right.

        // Triangle-left
        -0.5f, -0.5f, 0.0f, // Bottom-left.
        0.5f, 0.5f, 0.0f, // Top-right.
        -0.5f, 0.5f, 0.0f // Top-left.
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

    // 4: Set Viewport.
    glViewport(0, 0, 640, 460); // OpenGL parte da in basso a dx come origine.
    glClearColor(0.5f, 0.5f, 0.5f, 1.f);
    glUseProgram(ProgramId); // Diventa pipeline attuale di disegno.

    // Draw Point Mode: rasterizza solo i punti.
    // glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    // glPointSize(8);

    // Draw as Line: wireframe.
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(20); // DEPRECATED. Current GPU uses 1 and don't allow customize it.
}

Ex02QuadDraw::~Ex02QuadDraw()
{
    glDeleteVertexArrays(1, &Vao);
    glDeleteVertexArrays(1, &Vbo);
    glDeleteProgram(ProgramId);
}

void Ex02QuadDraw::Update(float DeltaTime)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6); // Prendiamo in considerazione 6 vertici totali, dallo 0.
    //glDrawArrays(GL_TRIANGLES, 0, 3); // Disegna triangolo sotto.
    //glDrawArrays(GL_TRIANGLES, 3, 6); // Disegna triangolo sopra.

}
