#pragma once

#include <string>
#include <glad/glad.h>

class OGLProgram
{
public:

    OGLProgram(const std::string& InVertexPath, const std::string& InFragPath);
    ~OGLProgram();
    void Bind();
    GLuint GetProgramID();

private:

    GLuint ProgramId;

};