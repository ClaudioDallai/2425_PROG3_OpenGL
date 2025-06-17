#include "OGLTexture.h"
#include <iostream>

// Essendo un single header library, almeno una volta dobbiamo definire l'implementazione con questo simbolo.
// Includere questa define in un cpp impedisce la duplicazione di simboli, cosa che invece sarebbe potuta succedere nell'header.
#define STB_IMAGE_IMPLEMENTATION 
#include "stb_image.h"

static GLuint CreateTexture(const std::string& InFilePath, bool FlipY);


OGLTexture::OGLTexture(const std::string& InImagePAth, const bool InFlipY)
{
    TextureId = CreateTexture(InImagePAth, InFlipY);
}

OGLTexture::~OGLTexture()
{
    // Eliminiamo la texture non più usata al destroy.
    glDeleteTextures(1, &TextureId); 
}


// Come per il programma e gli shader, creiamo una funzione che dal path, crea la texture e ne ritorna l'identificativo.
GLuint CreateTexture(const std::string& InFilePath, bool FlipY)
{
    // Per risolvere il problema del FlipY, possiamo agire sulla lettura dell'immagine invece che manualmente nel vertex shader.
    // Attenzione che questa APi è globale, quindi abilitarla la attiva globalmente anche per tutte le letture successive di altre immagini.
    // È un hoverhead molto piccolo poichè legge semplicemente i dati al contrario. Inoltre non è una operazione che viene fatta nell'Update.
    stbi_set_flip_vertically_on_load(FlipY);

    // Dato il patch come stringa C, viene tornato larghezza, altezza, canali, 
    // ed eventuali componenti (canali, quindi RGBA/RGB nel nostro caso) aggiuntivi da caricare (0 prende tutti canali).
    // Viene tornato un char* ai dati dei pixel.
    int Width, Height, Channels;
    unsigned char* Data = stbi_load(InFilePath.c_str(), &Width, &Height, &Channels, 0); 

    // Controllo che l'immagine sia stata letta correttamente.
    if (Data == NULL){
        std::cout << "Error Reading Image: " << InFilePath;
        throw std::runtime_error("Error Reading Image");
    }

    // Identifico il tipo di immagine dai canali.
    GLenum Format = Channels == 3 ? GL_RGB : GL_RGBA;


    // Creiamo una risorsa sulla GPU.

    // Creo N texture (1 nel nostro caso).
    GLuint TextureId;
    glGenTextures(1, &TextureId);

    // Per lavorare su questa immagine dobbiamo renderla come corrente (bind).
    // È lo stesso pattern del buffer VAO e VBO.
    glBindTexture(GL_TEXTURE_2D, TextureId);

    // Per caricare le immagini, dobbiamo stare attenti al numero di canali presi in considerazione.
    // Per proteggersi possiamo controllare il dato che ci ha fornito stbi_load.
    // Il formato specificato in Format è la tipologia che vogliamo dare alla risorsa texture. 
    // Il parametro di ingresso "format" invece è il formato richeisto della sorgente. Se sono uguali vuol dire che non richiediamo trasformazioni.

    // Upload verso la GPU.
    glTexImage2D(GL_TEXTURE_2D, 0, Format, Width, Height, 0, Format, GL_UNSIGNED_BYTE, Data); 

    // Wrapping.
    // Il wrapping su S, T, vuol dire sulle due dimensioni X, Y. In questo ambito sono rinominati S-T per non fare confusione.
    // Lo stesso discorso vale per esempio con le UV.
    // Il wrapping potrebbe quindi comportarsi in maniera diversa sulla X ed Y, ma in questo caso non serve, usiamo il repeat come metodologia.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Filtering.
    // Indichiamo che quando l'immagine si sta "minimizzando" per via del mipmap, 
    // deve scegliere linearmente il minmap stesso, e quando campiona internamente ad essa usa sempre un campionamento lineare (mischia i colori).
    // Quando diventa più grande invece, il comportamento sarà diverso, attraverso il nearest.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // MipMapping (opzionale).
    // Serve attivarlo poichè lo usiamo per il Filtering. Lo attiviamo sempre sulla texture attualmente bindata.
    glGenerateMipmap(GL_TEXTURE_2D);

    return TextureId;
}


void OGLTexture::Bind(GLuint TextureSlotID)
{
    glActiveTexture(TextureSlotID);
    glBindTexture(GL_TEXTURE_2D, TextureId); 
}

GLuint OGLTexture::GetTextureID()
{
    return TextureId;
}