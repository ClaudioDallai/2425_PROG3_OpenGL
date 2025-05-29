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

    // Il ProgramId diventa un campo della classe così da poterlo distruggere quando usciamo dal contesto.
    GLuint Vao;
    GLuint Vbo;
    GLuint Ebo;

    OGLProgram* Program;
};