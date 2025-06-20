#include <glad/glad.h>

class OGLProgram;
class OGLTexture;

class Ex08PhongDraw
{
public:
    Ex08PhongDraw();
    ~Ex08PhongDraw();
    void Update(float DeltaTime);
    GLuint ProgramId;

private:
    GLuint Vao;
    GLuint Vbo;
    GLuint Ebo;

    OGLProgram* Program;
    OGLTexture* BoxTexture;
};