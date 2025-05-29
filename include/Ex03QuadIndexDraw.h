#include <glad/glad.h>

class OGLProgram;

class Ex03QuadIndexDraw
{
public:

    Ex03QuadIndexDraw();
    ~Ex03QuadIndexDraw();
    void Update(float DeltaTime);

private:

    GLuint ProgramId;
    GLuint Vao;
    GLuint Vbo;
    GLuint Ebo;

    OGLProgram* Program;
};