#pragma once
#include <windows.h>
#include "dx11Displayer.h"

class UserInput
{
	public:
		int mouse(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		int keyboard(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		void setDisplayer(Dx11Displayer*);
	private:
		Dx11Displayer* pDisplayer;
};


