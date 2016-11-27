#include <windows.h>
#include "dx11Displayer.h"
#include "WinProcedure.h"
#include "controller.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	//	initial WinProcedure
	WinProcedure winPro(hInstance, nCmdShow, 1280, 800);

	//	Create Displayer
	Dx11Displayer caDisplayer(winPro.getHWND());

	//	Create UserInput
	UserInput userInput;
	userInput.setDisplayer(&caDisplayer);
	
	//	Create Animation Controller
	Controller controller;

	//	Setup WinProcedure
	winPro.setDisplayer(&caDisplayer);
	winPro.setUserInput(&userInput);
	winPro.setController(&controller);
	winPro.initial();

	return winPro.startLoop();
}



