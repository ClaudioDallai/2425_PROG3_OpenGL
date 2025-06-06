#include "OGLProgram.h"
#include <vector>
#include <fstream>
#include <iostream>

static std::string ReadFile(const std::string& InPath);
static GLuint CreateShader(const std::string& InPath, GLuint InShaderType);
static GLuint CreateProgram(GLuint VertexShaderId, GLuint FragmentShaderId);

// Useremo questa classe come API custom per rendere più usabile le sempre uguali chiamate per creare il programma.
// Wrappiamo poi via via che serve, tutto quello che ci è necessario per l'utilizzo.

OGLProgram::OGLProgram(const std::string& InVertexPath, const std::string& InFragPath)
{
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
    // Rendiamo il programma la pipeline attuale di disegno.
    glUseProgram(ProgramId);
}

GLuint OGLProgram::GetProgramID()
{
    return ProgramId;
}

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
