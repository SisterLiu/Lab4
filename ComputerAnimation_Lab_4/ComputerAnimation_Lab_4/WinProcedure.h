#pragma once
#include <windows.h>
#include "userInput.h"
#include "dx11Displayer.h"
#include "controller.h"

class WinProcedure
{
	public:
		int startLoop();
		void setUserInput(UserInput*);
		void setDisplayer(Dx11Displayer*);
		void setController(Controller*);
		WinProcedure(HINSTANCE hInstance, int nCmdShow, int screenX, int screenY);
		static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
		HWND getHWND();
		void initial();
	private:
		static HWND hwnd;
		static UserInput* pUserInput;
		static Dx11Displayer* pDisplayer;
		static Controller* pController;
		static int screenX;
		static int screenY;
		
		void render60();
		Object* createObjectFromMesh(Mesh* pMesh, float x, float y, float z, float angleX, float angleY, float angleZ);

		static std::vector<Object*> objects;
};




