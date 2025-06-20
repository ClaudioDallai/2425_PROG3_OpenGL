#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <format>
#include "XWindow.h"
#include "Ex01TriangleDraw.h"
#include "Ex02QuadDraw.h"
#include "Ex03QuadIndexDraw.h"
#include "Ex04QuadColorDraw.h"
#include "Ex05QuadTextureDraw.h"
#include "Ex06QuadPerspDraw.h"
#include "Ex07CubePerspDraw.h"
#include "Ex08PhongDraw.h"

#include "obj_parser.h"


int main() {
	std::cout << "It' Working" << std::endl;

	// Uso una classe Window creata da noi, che nel costruttore inizializza tutto ciò che serve.
	XWindow win = XWindow(640, 460, "Hello OpenGL");

	// Parametri utili per la frequenza di aggiornamento del titolo della finestra.
	float TitleUpdateMaxTime = 1.f;
	float TitleUpdateElapsed = 0.f;

	// Disattiva VSync, ovvero nessun intervallo per il Present-Operation tra i buffer.
	// win.SetVSync(false);

	// Esercizi OpenGL.
	// Ex01TriangleDraw Scene;
	// Ex02QuadDraw Scene;
	// Ex03QuadIndexDraw Scene;
	// Ex04QuadColorDraw Scene;
	// Ex05QuadTextureDraw Scene;
	// Ex06QuadPerspDraw Scene;
	// Ex07CubePerspDraw Scene;
	Ex08PhongDraw Scene;

	// Game Loop. 
	// Valido finchè il contesto della finestra non viene chiuso esplicitamente.
	while (win.IsOpened())
	{
		// Prendo il DeltaTime dalla nostra finestra.
		float DeltaTime = win.GetDeltaTime();

		// Ogni MaxTime specificato attraverso un contatore aggiorno il titolo della finestra.
		TitleUpdateElapsed += DeltaTime;
		if (TitleUpdateElapsed >= TitleUpdateMaxTime){

			// Calcolo FPS.
			// Il VSync è attivo di base, quindi gli fps uscenti saranno limitati al target Hertz del monitor.
			int Fps = 1.f / DeltaTime;

			// Creo titolo finestra usato <format> C++20.
			std::string FormattedTitle = std::format("OpenGL App | DeltaTime: {} - FPS: {}", DeltaTime, Fps);

			// Imposto il titolo della finestra.
			// Devo prendere la stringa in versione C (char*) dalla stringa C++ usando c_str().
			win.SetTitle(FormattedTitle);

			// Resetto l'accumulatore tornando indietro di 1 secondo (senza impostare a 0), essendo sicuri di avere intervalli di 1 secondo.
			// Garantiamo quindi ogni MaxTime l'aggiornamento.
			TitleUpdateElapsed -= TitleUpdateMaxTime;
		}

		Scene.Update(DeltaTime);

		// Aggiornamento della finestra. Questo Update include anche le funzioni per Poll e SwapBuffer.
		win.Update();
	}

	return 0;	
}