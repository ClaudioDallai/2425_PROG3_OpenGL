#include <glad/glad.h>


class Ex02QuadDraw
{
public:

    Ex02QuadDraw();
    ~Ex02QuadDraw();
    void Update(float DeltaTime);

private:

    GLuint ProgramId;
    GLuint Vao;
    GLuint Vbo;
};