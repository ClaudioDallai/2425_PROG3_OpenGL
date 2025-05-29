#include "Ex02QuadDraw.h"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

static std::string ReadFile(const std::string& InPath)
{
    std::ifstream InputStream(InPath, std::ios::ate);
    size_t FileSize = InputStream.tellg();

    std::string Text;
    Text.resize(FileSize);

    InputStream.seekg(0, std::ios::beg);
    InputStream.read(&Text[0], FileSize);

    InputStream.close();
    return Text;
}

static GLuint CreateShader(const std::string& InPath, GLuint InShaderType)
{
    std::string Text = ReadFile(InPath);
    const char* ShaderSource = Text.c_str();

    GLuint ShaderId = glCreateShader(InShaderType);
    glShaderSource(ShaderId, 1, &ShaderSource, NULL);

    glCompileShader(ShaderId);

    GLint Success;
    glGetShaderiv(ShaderId, GL_COMPILE_STATUS, &Success);

    // 0 -> falso; 1 -> true.
    if (!Success)
    {
        GLint MaxLogLength;
        glGetShaderiv(ShaderId, GL_INFO_LOG_LENGTH, &MaxLogLength);
        
        std::vector<GLchar> InfoLog(MaxLogLength);
        glGetShaderInfoLog(ShaderId, MaxLogLength, NULL, InfoLog.data());

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

    glDeleteShader(VertexShaderId);
    glDeleteShader(FragmentShaderId);

    return ProgramId;
}

Ex02QuadDraw::Ex02QuadDraw()
{
    GLuint VertexShaderId = CreateShader("resources/shaders/triangle.vert", GL_VERTEX_SHADER);
    GLuint FragmentShaderId = CreateShader("resources/shaders/triangle.frag", GL_FRAGMENT_SHADER);

    ProgramId = CreateProgram(VertexShaderId, FragmentShaderId);

    // Per creare un quadrato, possiamo trattarlo come due triangoli del tipo: |/ /| 
    std::vector<float> Vertices = {
        // Triangle-right:
        -0.5f, -0.5f, 0.0f, // Bottom-left.
        0.5f, -0.5f, 0.0f,  // Bottom-right.
        0.5f, 0.5f, 0.0f,   // Top-right.

        // Triangle-left:
        -0.5f, -0.5f, 0.0f, // Bottom-left.
        0.5f, 0.5f, 0.0f,   // Top-right.
        -0.5f, 0.5f, 0.0f   // Top-left.
    };

    // Passiamo i dati alla GPU.

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


    // 4: Impostiamo la Viewport.
    glViewport(0, 0, 640, 460);
    glClearColor(0.5f, 0.5f, 0.5f, 1.f);
    glUseProgram(ProgramId);


    // Esistono anche delle direttive per cambiare come la GPU rasterizza.

    // Draw Point Mode: rasterizza solo i punti.
    // glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    // glPointSize(8);

    // Draw As Line: wireframe.
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // glLineWidth(20); // DEPRECATED. Current GPU uses 1 and doesn't allow customize it.
}

Ex02QuadDraw::~Ex02QuadDraw()
{
    glDeleteVertexArrays(1, &Vao);
    glDeleteBuffers(1, &Vbo);
    glDeleteProgram(ProgramId);
}

void Ex02QuadDraw::Update(float DeltaTime)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6); // Prendiamo in considerazione 6 vertici totali, dallo 0.

    // Possiamo anche vedere i due triangoli disegnati singolarmente.
    //glDrawArrays(GL_TRIANGLES, 0, 3); // Disegna triangolo sotto.
    //glDrawArrays(GL_TRIANGLES, 3, 6); // Disegna triangolo sopra.
}
