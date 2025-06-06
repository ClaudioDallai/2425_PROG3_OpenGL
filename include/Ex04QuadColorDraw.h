#include <glad/glad.h>

class OGLProgram;

class Ex04QuadColorDraw
{
public:
    Ex04QuadColorDraw();
    ~Ex04QuadColorDraw();
    void Update(float DeltaTime);
    GLuint ProgramId;

private:
    GLuint Vao;
    GLuint Vbo;
    GLuint Ebo;

    OGLProgram* Program;
};