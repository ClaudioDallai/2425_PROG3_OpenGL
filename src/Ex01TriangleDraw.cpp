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

// Dobbiamo linkare gli shader con un programma. 
// Anche questo ha un identificativo, e per linkare avrà bisogno di entrambi gli shader.
static GLuint CreateProgram(GLuint VertexShaderId, GLuint FragmentShaderId)
{
    // Creiamo un programma come risorsa vuota su GPU.
    GLuint ProgramId = glCreateProgram();

    // Attacchiamo gli shader al ProgramID creato.
    glAttachShader(ProgramId, VertexShaderId);
    glAttachShader(ProgramId, FragmentShaderId);

    // Linkiamo il programma.
    glLinkProgram(ProgramId);

    // Stampiamo eventuali errori lato GPU, seguiamo la stessa logica già vista.
    // Cambiano giusto alcuni nomi delle API poichè si riferiscono al program.
    GLint Success;
    glGetProgramiv(ProgramId, GL_LINK_STATUS, &Success);
    if (!Success)
    {
        GLint MaxLogLength;
        glGetShaderiv(ProgramId, GL_INFO_LOG_LENGTH, &MaxLogLength);

        std::vector<GLchar> InfoLog(MaxLogLength);
        glGetProgramInfoLog(ProgramId, MaxLogLength, NULL, InfoLog.data());

        std::string LogStr(InfoLog.begin(), InfoLog.end());
        std::cout << "[ERRO] Program Linking failure: " << LogStr;
        throw std::runtime_error(LogStr);
    }

    // Una volta creato il programma, le risorse degli shader possono essere cancellate.
    // Abbiamo creato il programma e compilato, le "librerie statiche" come similitudine agli shader, non sono più necessarie.
    glDeleteShader(VertexShaderId);
    glDeleteShader(FragmentShaderId);

    // Se è quindi tutto ok, ritorniamo il programma.
    return ProgramId;
}

Ex01TriangleDraw::Ex01TriangleDraw()
{
    // Creiamo i due shader.
    // L'estensione in realtà può essere qualsiasi, usiamo .vert e .frag per chiarezza. 
    // Dobbiamo indicare però la loro tipologia effettiva, ovvero come vanno interpretati i loro dati.
    GLuint VertexShaderId = CreateShader("resources/shaders/triangle.vert", GL_VERTEX_SHADER);
    GLuint FragmentShaderId = CreateShader("resources/shaders/triangle.frag", GL_FRAGMENT_SHADER);

    // Creiamo il programma dagli shader.
    ProgramId = CreateProgram(VertexShaderId, FragmentShaderId);

    // Identifichiamo i vertici. Serve preparare dei dati da inviare alla GPU.
    // Questi andranno immagazzinati come successione continua (stream) di float, non vi è suddivisione vera e propria, lo "human-readable" serve a noi.

    // Andranno specificati in senso anti-orario. Attenzione che il loro spazio di riferimento è NDC.
    // Seguono quindi la regola della mano sinistra. La Z non è importante, può sempre rimanere 0.0f.

    std::vector<float> Vertices = {
        -0.5f, -0.5f, 0.0f, // Bottom-left.
        0.5f, -0.5f, 0.0f,  // Bottom-right.
        0.0f, 0.5f, 0.0f    // Top.
    };

    // Dobbiamo adesso passare i dati del triangolo alla GPU.


    // 1: Creiamo un VAO (Vertex Array Object). È un "portachiavi" di riferimento che controlla e si riferisce a più buffer contemporaneamente.
    // Attivando questo VAO come corrente, attiveremo tutti gli altri buffer che si riferiscono ad esso. È quindi un gestore di buffer.
    glGenVertexArrays(1, &Vao); // Creiamo il VAO. Tutte queste creazioni possono essere multiple, col primo parametri ne specifichiamo la quantità.
    glBindVertexArray(Vao); // Una volta bindato, tutti gli altri buffer che creeremo si riferiranno al VAO attualmente bindato. È una macchina a stati.
    
    // Attenzione nel caso in cui ci sono più VAO. Se sbagliamo o ci dimentichiamo quello attualmente bindato, tutti gli altri buffer faranno riferimento ad esso,
    // il che provoca errori nei dati e nelle visualizzazioni di essi.



    // 2: Creiamo un VBO (Vertex Buffer Object). Viene usato per caricare i dati dei vertici.
    glGenBuffers(1, &Vbo); // Creiamo il buffer VBO.
    glBindBuffer(GL_ARRAY_BUFFER, Vbo); // In base a dove lo bindiamo ed il tipo, la GPU fa operazioni diverse. In questa maniera il VBO è legato al VAO attualmente bindato.
    // Se non lo bindassimo, questo non sarebbe quello corrente preso in considerazione, 
    // quindi rischieremmo di caricare dati su un buffer precedente che non c'entra nulla con questa esecuzione su GPU.

    // Dobbiamo indicare alla GPU quanto questo buffer VBO è grande.
    size_t DataSize = Vertices.size() * sizeof(float);
    // Indichiamo alla GPU che nel buffer che ha attivo correntemente, ci saranno DataSize dati, i dati stessi, ed una specifica per la gestione. 
    // GL_STATIC_DRAW indica che lo leggeremo spesso ma non verrà aggiornato.
    glBufferData(GL_ARRAY_BUFFER, DataSize, Vertices.data(), GL_STATIC_DRAW); 



    // 3: Linkiamo il buffer con i relativi dati al Vertex Shader.
    // Il Vertex Shader è come una "scatola". Cerchiamo di legargli degli input (AttribPointers).
    // Dobbiamo indicare allo shader che in un "pin" di ingresso specifico arriva il singolo vertice. 
    // Ci penserà poi la GPU ad eseguire il Vertex Shader N volte, uno per vertice.

    GLuint Location_0 = 0;

    // In posizione 0, indichiamo quanti elementi passiamo a quella variabile (3 elementi per volta identificano un vertice).
    // Indichiamo poi che questi elementi sono di tipo float, senza normalizzazione (trasformazione dati al passaggio, non le vogliamo).
    // Passando una sequenza continua di float, dobbiamo indicare alla GPU ogni quanto passiamo da un verttice ad un altro, e quindi indichiamo la dimensione di 3 float.
    // Infine nell'ultimo parametro indichiamo che deve partire dall'inizio, è un offset che deve essere mantenuto castato a void* per retro-compatibilità.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // Ho quindi definito la variabile che mi aspetto sia dentro il VertexShader come "pin" di ingresso. Questa effettivamente deve essere presente ovviamente.
    // Apro il suddetto pin (pin posizione 0). Per non fare confusione, invece di scrivere 0, chiamo la variabile come poi si chiamerà sullo shader.
    glEnableVertexAttribArray(Location_0);



    // 4: Imposto la Viewport di disegno.
    glViewport(0, 0, 640, 460); // Indichiamo alla GPU la dimensione e posizione di rasterizzazione. OpenGL parte da in basso a dx come origine.
    glClearColor(0.5f, 0.5f, 0.5f, 1.f); // ClearColor della Viewport.
    glUseProgram(ProgramId); // Faccio diventare il programma creato la pipeline attuale di disegno.
}

Ex01TriangleDraw::~Ex01TriangleDraw()
{
    // Distruggiamo il programma ed i vari buffer.
    glDeleteVertexArrays(1, &Vao);
    glDeleteBuffers(1, &Vbo);
    glDeleteProgram(ProgramId);
}

void Ex01TriangleDraw::Update(float DeltaTime)
{
    // Puliamo lo schermo (in realtà il back_buffer). 
    // Puliamo solo il color_buffer al momento. Più avanti potrà essere pulito anche lo Z_buffer. Accetta infatti una bitmask con i flag in or bit.
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw-call in modalità triangolo, partendo dal primo ed il numero massimo di vertici. 
    // 3 vertici sono il numero minimo per completare un triangolo, altrimenti non disegnerà niente.
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // ATTENZIONE: Ricordo però che per produrre pixel dovrà essere specificato anche un colore. Faccio ciò nel codice stesso del Fragment Shader.
}
