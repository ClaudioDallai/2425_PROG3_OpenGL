#include <glad/glad.h>


class Ex02QuadDraw
{
public:
    Ex02QuadDraw();
    ~Ex02QuadDraw();
    void Update(float DeltaTime);

private:

    // Il ProgramId diventa un campo della classe così da poterlo distruggere quando usciamo dal contesto.
    GLuint ProgramId;
    GLuint Vao;
    GLuint Vbo;
};