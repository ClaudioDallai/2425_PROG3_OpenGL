#include <glad/glad.h>

class OGLProgram;
class OGLTexture;

class Ex07CubePerspDraw
{
public:
    Ex07CubePerspDraw();
    ~Ex07CubePerspDraw();
    void Update(float DeltaTime);
    GLuint ProgramId;

private:
    GLuint Vao;
    GLuint Vbo;
    GLuint Ebo;

    OGLProgram* Program;
    OGLTexture* BoxTexture;
};