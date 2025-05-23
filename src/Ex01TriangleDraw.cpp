#include "Ex01TriangleDraw.h"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

// Funzione per leggere file. Dovremmo leggere i file contenenti gli shader grezzi.
static std::string ReadFile(const std::string& InPath)
{
    // Ci serve caricare tutto il file in memoria come stringa, poichè dobbiamo compilarlo.
    // Lo shader ha bisogno della compilazione per essere usato.
    // Poichè dovremmo leggere tutto, per non dover concatenare i buffer di appoggio con la stringa da costruire,
    // dovremmo sapere quanto è grande il file. Potremmo allocare totalmente il buffer e leggere tutto insieme, un "ReadAll".
    // Possiamo aprire il file, portare alla fine il cursore (apertura ate ovvero at the end), 
    // e posso chiedere allo stream quanto il cursore è stato scorso. Quello è il filesize.

    std::ifstream InputStream(InPath, std::ios::ate);
    size_t FileSize = InputStream.tellg(); // Ritorna la posizione del cursore in Byte.

    std::string Text;
    Text.resize(FileSize);

    // Dopo aver saputo quanto è grande il file per allocare il buffer, risposto il cursore all'inizio (beg ovvero begin).
    // Proseguo a leggere il file partendo dal primo byte (&Text[0]), fino a FileSize bytes.
    InputStream.seekg(0, std::ios::beg); // Sposta il cursore. I parametri sono: offset a partire da dove indicato (dall'inizio in questo caso).
    InputStream.read(&Text[0], FileSize);

    InputStream.close();
    return Text;
}

// Devo creare lo shader. Devo distinguere se è di tipo Frag (agisce e ricostruisce i vertici) o Vert (agisce e colora i pixel).
static GLuint CreateShader(const std::string& InPath, GLuint InShaderType)
{
    std::string Text = ReadFile(InPath);
    const char* ShaderSource = Text.c_str(); // Puntatore al sorgente dello shader letto precedentemente (le API OpenGL sono C, per questo serve char*).

    // Iniziamo a dialogare con la GPU.
    // Creiamo lo Shader del tipo specificato e carichiamo i sorgenti sulla GPU.
    GLuint ShaderId = glCreateShader(InShaderType); // Ritorna l'ID della risorsa shader creata.

    // Leghiamo allo shader il sorgente indicando: 
    // il suo id, quanti shader stiamo compilando, un riferimento al sorgente char*, lunghezza del sorgente (con NULL prende tutto).
    glShaderSource(ShaderId, 1, &ShaderSource, NULL); 

    // Compiliamo lo shader indicando l'id.
    glCompileShader(ShaderId);

    // Essendo compilato su GPU, se fallisce non vedremo niente. Dobbiamo chiedere alla GPU se il tutto è riuscito.
    // Sfruttiamo API di richiesta informazioni.
    GLint Success;
    glGetShaderiv(ShaderId, GL_COMPILE_STATUS, &Success); // Chiediamo lo status della compilazione.

    // 0 -> false; 1 -> true.
    if (!Success)
    {
        // Chiediamo quali errori di compilazione sono avvenuti. 
        // Stampandoli a schermo possiamo risolverli.
        
        // Chiediamo alla GPU quanto è grosso il più grande LOG scrivibile, così da evitare successivi problemi in lettura.
        // Salviamo questo dato nel buffer appena creato. 
        GLint MaxLogLength; 
        glGetShaderiv(ShaderId, GL_INFO_LOG_LENGTH, &MaxLogLength); 
        
        // Otteniamo un puntatore di char dal vettore MODIFICABILE, così ci possiamo scrivere dentro. Il char* sarebbe costante e non potremmo scriverci dopo. 
        // Possiamo scrivere i mess di errore dall'inizio del puntatore fino alla fine della grandezza MaxLogLength.
        // Essendo il vettore sequenziale, possiamo scrivere dall'inizio alla fine senza problemi.
        std::vector<GLchar> InfoLog(MaxLogLength);
        glGetShaderInfoLog(ShaderId, MaxLogLength, NULL, InfoLog.data());

        // Stampiamo le informazioni a schermo. Costruiamo la stringa dal vettore, usando gli iteratori.
        std::string LogStr(InfoLog.begin(), InfoLog.end());
        std::cout << "[ERRO] Shader compilation failure: " << LogStr;
        throw std::runtime_error(LogStr);
    }

    // Se tutto è ok invece, ritorniamo l'identificativo dello shader creato.
    return ShaderId;
}

static GLuint CreateProgram(GLuint VertexShaderId, GLuint FragmentShaderId)
{
    GLuint ProgramId = glCreateProgram();
    glAttachShader(ProgramId, VertexShaderId);
    glAttachShader(ProgramId, FragmentShaderId);
    glLinkProgram(ProgramId);

    // Stampiamo eventuali errori GPU, stessa logica vista.
    GLint Success;
    glGetProgramiv(ProgramId, GL_LINK_STATUS, &Success);
    if (!Success)
    {
        GLint MaxLogLength;
        glGetShaderiv(ProgramId, GL_INFO_LOG_LENGTH, &MaxLogLength);

        std::vector<GLchar> InfoLog(MaxLogLength);
        glGetProgramInfoLog(ProgramId, MaxLogLength, NULL, InfoLog.data());

        std::string LogStr(InfoLog.begin(), InfoLog.end());
        std::cout << "[ERRO] Shader Linking failure: " << LogStr;
        throw std::runtime_error(LogStr);

    }

    // Una volta creato il programma, le risorse degli shader possono essere cancellate.
    // Abbiamo creato il programma e compilato, le "librerie statice", come similutudine agli shader, non sono più necessari.
    glDeleteShader(VertexShaderId);
    glDeleteShader(FragmentShaderId);

    return ProgramId;
}

Ex01TriangleDraw::Ex01TriangleDraw()
{
    // L'estensione può essere qualsiasi, usaimo vert e frag per chiarezza.
    GLuint VertexShaderId = CreateShader("resources/shaders/triangle.vert", GL_VERTEX_SHADER);
    GLuint FragmentShaderId = CreateShader("resources/shaders/triangle.frag", GL_FRAGMENT_SHADER);

    ProgramId = CreateProgram(VertexShaderId, FragmentShaderId);

    // Counter-clock wise. By now in vertex data in NDC space.
    // Left-handed rule. But Z is 0 so is not important.
    std::vector<float> Vertices = {
        -0.5f, -0.5f, 0.0f, // Bottom-left.
        0.5f, 0-.5f, 0.0f, // Bottom-right.
        0.0f, 0.5f, 0.0f // Top.
    };

    // Dobbiamo passare i dati del triangolo alla GPU.

    // 1: Create VAO. "Portachiavi" di riferimento che si riferisce e controlla più buffer contemporaneamente.
    glGenVertexArrays(1, &Vao); //Creiamolo.
    glBindVertexArray(Vao); //Una volta bindato, tutti i buffer che creeremo si riferiranno al VAO attualmente bindato.

    // 2: Create VBO. Usato per caricare i dati dei vertici.
    glGenBuffers(1, &Vbo); // Creiamo il buffer.
    glBindBuffer(GL_ARRAY_BUFFER, Vbo); // In base a dove lo bindiamo ed il tipo, la GPU fa operazioni diverse. In questa maniera il VBO è legato al VAO bindato.
    // Se non bindassimo questo non sarebbe quello corrente preso in considerazione, quindi rischieremmo di caricare dati su un buffer precedente che non c'entra nulla con questa esecuzione su GPU.

    // Indichiamo alla GPU quanto questo buffer è grande.
    size_t DataSize = Vertices.size() *sizeof(float);
    glBufferData(GL_ARRAY_BUFFER, DataSize, Vertices.data(), GL_STATIC_DRAW); // Indichiamo alla GPU il buffer ed anche come useremo il buffer. GL_STATIC_DRAW indica che lo leggeremo spesso ma non verrà aggiornato.

    // 3: Link Buffer to Vertex Shader.
    GLuint Location_0 = 0;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(Location_0);

    // 4: Set Viewport.
    glViewport(0, 0, 640, 460); // OpenGL parte da in basso a dx come origine.
    glClearColor(0.5f, 0.5f, 0.5f, 1.f);
    glUseProgram(ProgramId); // Diventa pipeline attuale di disegno.

}

Ex01TriangleDraw::~Ex01TriangleDraw()
{
    glDeleteVertexArrays(1, &Vao);
    glDeleteVertexArrays(1, &Vbo);
    glDeleteProgram(ProgramId);
}

void Ex01TriangleDraw::Update(float DeltaTime)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}
