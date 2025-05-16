#include <glad/glad.h>

class OGLProgram;

class Ex03QuadIndexDraw
{
public:
    Ex03QuadIndexDraw();
    ~Ex03QuadIndexDraw();
    void Update(float DeltaTime);

private:

    // Il ProgramId diventa un campo della classe così da poterlo distruggere quando usciamo dal contesto.
    GLuint ProgramId;
    GLuint Vao;
    GLuint Vbo;
    GLuint Ebo;

    OGLProgram* Program;
};