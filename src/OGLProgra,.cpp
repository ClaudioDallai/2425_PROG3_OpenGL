#include "OGLProgram.h"
#include <vector>
#include <fstream>
#include <iostream>

static std::string ReadFile(const std::string& InPath);
static GLuint CreateShader(const std::string& InPath, GLuint InShaderType);
static GLuint CreateProgram(GLuint VertexShaderId, GLuint FragmentShaderId);


OGLProgram::OGLProgram(const std::string& InVertexPath, const std::string& InFragPath)
{
    // L'estensione può essere qualsiasi, usaimo vert e frag per chiarezza.
    GLuint VertexShaderId = CreateShader(InVertexPath, GL_VERTEX_SHADER);
    GLuint FragmentShaderId = CreateShader(InFragPath, GL_FRAGMENT_SHADER);

    ProgramId = CreateProgram(VertexShaderId, FragmentShaderId);
}

OGLProgram::~OGLProgram()
{
    glDeleteProgram(ProgramId);
}

void OGLProgram::Bind()
{
    glUseProgram(ProgramId); // Diventa pipeline attuale di disegno.
}

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