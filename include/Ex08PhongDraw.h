#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>


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
    OGLTexture* StormTexture;

    std::vector<float> Vertices;

    glm::mat4 View;
    glm::mat4 Projection;
};