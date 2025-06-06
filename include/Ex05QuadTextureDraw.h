#include <glad/glad.h>

class OGLProgram;

class Ex05QuadTextureDraw
{
public:
    Ex05QuadTextureDraw();
    ~Ex05QuadTextureDraw();
    void Update(float DeltaTime);
    GLuint ProgramId;

private:
    GLuint Vao;
    GLuint Vbo;
    GLuint Ebo;

    OGLProgram* Program;

    GLuint SmileTextureId;
    GLuint BoxTextureId;
};