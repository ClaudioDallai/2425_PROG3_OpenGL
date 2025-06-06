#include "Ex05QuadTextureDraw.h"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include "OGLProgram.h"

#define STB_IMAGE_IMPLEMENTATION // Essendo un single header library, almeno una volta dobbiamo definire l'implementazione con questo simbolo.
#include "stb_image.h"

// Vogliamo leggere una texture.
// Per leggere una immagine sfruttiamo stb_image con la API di load. 
// Una volta ottenuto il buffer, dobbiamo creare una etxture su GPU e caricarci i dati che abbiamo ottenuto.

struct Color
{
    float R;
    float G;
    float B;
    float A;
};

// Come per il programma e gli shader, creiamo una funzione che dal path, crea la texture e ne ritorna l'identificativo.
GLuint CreateTexture(const std::string& InFilePath, bool flipY)
{
    // Per risolvere il problema del flipY, possiamo agire sulla lettura dell'immagine invece che manualmente nel vertex shader.
    // Attenzione che questa APi è globale, quindi abilitarla la attiva globalmente anche per tutte le letture successive di altre immagini.
    // È un hoverhead molto piccolo poichè legge semplicemente i dati al contrario. Inoltre non è una operazione che viene fatta nell'Update.
    stbi_set_flip_vertically_on_load(flipY);


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

Ex05QuadTextureDraw::Ex05QuadTextureDraw()
{
    // Usiamo nuovi shader che gestiscono texture, ovvero hanno informazioni e calcoli anche sulle UV.
    Program = new OGLProgram("resources/shaders/quadtexture.vert", "resources/shaders/quadtexture.frag");

    // Invece dei colori, gestiamo le UVs.
    std::vector<float> Vertices = {
        // Triangle-right    // UVs
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,     // Bottom-left.
        0.5f, -0.5f, 0.0f,   1.0f, 0.0f,     // Bottom-right.
        0.5f, 0.5f, 0.0f,    1.0f, 1.0f,     // Top-right.
        // Triangl
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,     // Bottom-left.
        0.5f, 0.5f, 0.0f,    1.0f, 1.0f,     // Top-right.
        -0.5f, 0.5f, 0.0f,   0.0f, 1.0f      // Top-left.
    };

    // Dobbiamo passare i dati del triangolo alla GPU.

    // 1: VAO.
    glGenVertexArrays(1, &Vao);
    glBindVertexArray(Vao);



    // 2: VBO.
    glGenBuffers(1, &Vbo);
    glBindBuffer(GL_ARRAY_BUFFER, Vbo);

    // Indichiamo alla GPU quanto questo buffer è grande.
    size_t DataSize = Vertices.size() *sizeof(float);
    glBufferData(GL_ARRAY_BUFFER, DataSize, Vertices.data(), GL_STATIC_DRAW); 



    // 3: Link del Buffer al Vertex Shader. 
    // Dobbiamo leggere correttamente dall'array, tenendo in considerazione delle UVs. Le posizioni vengono prese di 5 in 5 partendo da 0.
    GLuint Location_0 = 0;
    glVertexAttribPointer(Location_0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(Location_0);

    // Leggo i dati per il nuovo pin UV.
    // Prendo le UV di 5 in 5, partendo dal terzo, leggendo due alla volta.
    GLuint Location_1 = 1;
    glVertexAttribPointer(Location_1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(Location_1);



    // 4: Viewport.
    glViewport(0, 0, 640, 460);
    glClearColor(0.5f, 0.5f, 0.5f, 1.f);


    Program->Bind();


    // 5: Creiamo le immagini. 
    // Attenzione che le texture, quando non le usiamo più vanno distrutte.
    SmileTextureId = CreateTexture("resources/textures/smile.png", true);
    BoxTextureId = CreateTexture("resources/textures/wood-box.jpg", true);

    // Rendiamo attuale la texture creata.
    // Va abilitato lo slot della texture. La GPU ha la texture caricata in memoria, ma va impostata attiva per gli slot degli shader.
    // Gli slot vanno attivati in maniera ordinata, partendo dallo 0.
    glActiveTexture(GL_TEXTURE0);

    // Potrebbe non essere necessario poichè la bindiamo nel CreateTexture, ma ripetere l'operazione aggiorna in maniera corretta la macchina a stati, evitando problemi.
    glBindTexture(GL_TEXTURE_2D, SmileTextureId); 


    // La texture è costante per ogni vertice. 
    // Quindi può essere rappresentata con una uniform. Possiamo crearla direttamente dove serve, quindi nel .frag in questo caso.
    // Il valore 0 nella funzione indica lo slot che abbiamo attivato. Quindi assegniamo il valore 0 alla uniform. In modo da attaccare il Sampler2D a quello slot.
    // glUniform1i(glGetUniformLocation(Program->GetProgramID(), "smile_tex"), 0); 

    // ATTENZIONE: Alternativamente questo bind ella uniform allo slot può essere effettuato nello shader con la direttiva di binding.
    // È ciò che avviene in questo esempio, e quindi quello scritto sopra non è più necessario. Possiamo usare una delle due modalità.
    // Il funzionamento è quindi spostato totalmente nel .vert.



    // Lavoriamo adesso anche su un'altra immagine creata in contemporanea alla prima.
    // Questa volta ci dobbiamo collegare allo slot 1.
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, BoxTextureId); 
    
    // Nello shader fragment, possiamo anche giocare con queste texture, per esempio interpolandole tra loro.
    // Per controllare la trasparenza, quindi l'Alpha-Blending, dobbiamo attivarla in maniera esplicita.

    // 6: Attivare AlphaBlending.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Specifichiamo la funzione di blend che vogliamo usare. 1-X è un AlphaBlending classico.
}

Ex05QuadTextureDraw::~Ex05QuadTextureDraw()
{
    glDeleteVertexArrays(1, &Vao);
    glDeleteVertexArrays(1, &Vbo);
    glDeleteTextures(1, &SmileTextureId); // Eliminiamo le texture non usate.
    glDeleteTextures(1, &BoxTextureId);
    delete Program;
}

void Ex05QuadTextureDraw::Update(float DeltaTime)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
