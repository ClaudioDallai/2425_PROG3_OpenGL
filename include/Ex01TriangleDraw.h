#include <glad/glad.h>

class Ex01TriangleDraw
{
public:
    Ex01TriangleDraw();
    ~Ex01TriangleDraw();
    void Update(float DeltaTime);

private:

    // Parametri di classe. Li salviamo in modo da poterli riutilizzare per successive chiamate API ad OpenGL.
    GLuint ProgramId;
    GLuint Vao;
    GLuint Vbo;
};