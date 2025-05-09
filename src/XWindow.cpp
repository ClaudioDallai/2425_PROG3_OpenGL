#include "XWindow.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdexcept>

#pragma region Special_Member_Functions

XWindow::XWindow(int InWidth, int InHeight, const std::string InTitle)
    : Width(InWidth), Height(InHeight), Title(InTitle), DeltaTime(0.f)
{ 

    // Controllo che glfw sia inizializzato.
	if (glfwInit() == GLFW_FALSE){
		throw std::runtime_error("Error glfw init");
	}

	// Creo contesto grafico. 
	// Devo effettuare un setup attraverso degli hint (suggerimenti sulla configurazione, se fallisce prova ad usare altro).

	// Versione 4.6 -> identificato come Major.Minor.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	// Creo finestra secondo specifiche. Prendo dati dal costruttore.
	GLFWmonitor* monitor = NULL; // Devo specificare fisicamente il monitor usato. Se NULL usa il primario.

	RawWindow = glfwCreateWindow(Width, Height, Title.c_str(), monitor, NULL);

	if (RawWindow == NULL){
		throw std::runtime_error("Error creating window");
	}

    // Rendiamo il contesto della finestra creata come il corrente, in modo da poterci invocare metodi.
	glfwMakeContextCurrent(RawWindow);

    if (gladLoadGL() == 0){
		throw std::runtime_error("Error loading GL Functions");
	}
}

XWindow::~XWindow()
{
    // Al fuori scope, la finestra viene liberata in automatico.
    glfwDestroyWindow(RawWindow);
}

#pragma endregion Special_Member_Functions

#pragma region Window_Utility

int XWindow::IsOpened() const
{
    return glfwWindowShouldClose(RawWindow) ? 0 : 1;
}

void XWindow::SetTitle(const std::string InTitle)
{
    glfwSetWindowTitle(RawWindow, InTitle.c_str());     
}

void XWindow::Update()
{
    // Calcolo DeltaTime.
	static float LastTime = glfwGetTime(); // Restituisce il tempo in questo momento. Lo salvo in una var statica per calcolare il DeltaTime in iterazioni diverse.
    float CurrTime = glfwGetTime();
	DeltaTime = CurrTime - LastTime;
	LastTime = CurrTime;

    // Operazione di "Present" dei buffer. È una operazione GPU che inverte il puntatore del buffer (front/back).
    glfwSwapBuffers(RawWindow); 

    // Ci permette di rendere disponibili ed usare i vari eventi I/O.
	glfwPollEvents(); 
}

float XWindow::GetDeltaTime() const
{
    return DeltaTime;
}

void XWindow::SetVSync(bool InEnabled)
{
    // Accetta un intervallo VSync. 
    // Con un 0-1 sostanzialmente abbiamo un comportamento di acceso-spento.
    
    if (!InEnabled){
        glfwSwapInterval(0);
    } else {  
        glfwSwapInterval(1);
    }
}

#pragma endregion Window_Utility
