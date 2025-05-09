#include <string>

// Forward Declaration (è un tipo C quindi non posso usare 'class').
typedef struct GLFWwindow GLFWwindow;

// Classe per la finestra di gioco.
class XWindow
{
public:
    // Costruttore e distruttore. 
    XWindow(int InWidth, int InHeight, const std::string InTitle);
    ~XWindow();
    
    // Metodi utili della finestra. Sfrutteranno internamente le API di OpenGL.
    int IsOpened() const;
    void SetTitle(const std::string InTitle);
    void Update();
    float GetDeltaTime() const;
    void SetVSync(bool InEnabled);

private:
    // Parametri della finestra. 
    int Width;
    int Height;
    std::string Title;
    GLFWwindow* RawWindow;
    float DeltaTime;
    
};