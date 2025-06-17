#include <glad/glad.h>

class OGLProgram;
class OGLTexture;

class Ex06QuadPerspDraw
{
public:
    Ex06QuadPerspDraw();
    ~Ex06QuadPerspDraw();
    void Update(float DeltaTime);
    GLuint ProgramId;

private:
    GLuint Vao;
    GLuint Vbo;
    GLuint Ebo;

    OGLProgram* Program;

    OGLTexture* SmileTexture;
    OGLTexture* BoxTexture;
};