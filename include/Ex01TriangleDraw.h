#include <glad/glad.h>


class Ex01TriangleDraw
{
public:
    Ex01TriangleDraw();
    ~Ex01TriangleDraw();
    void Update(float DeltaTime);

private:

    // Il ProgramId diventa un campo della classe così da poterlo distruggere quando usciamo dal contesto.
    GLuint ProgramId;
    GLuint Vao;
    GLuint Vbo;
};