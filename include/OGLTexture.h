#pragma once

#include <string>
#include <glad/glad.h>

// Questa classe, allo stesso modo di OGLProgram, si occuperà di creare e gestire le texture, 
// attraverso il metodo precedentemente fatto, così da evitare ripetizione di codice.

class OGLTexture
{
public:

    OGLTexture(const std::string& InImagePAth, const bool InFlipY);
    ~OGLTexture();
    void Bind(GLuint TextureSlotID);
    GLuint GetTextureID();

private:

    GLuint TextureId;

};