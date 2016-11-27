#include "userInput.h"

int UserInput::mouse(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

int UserInput::keyboard(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_KEYDOWN:
			switch(wParam)
			{
				case 'W':
					pDisplayer->cameraControl.GO_FRONT = true;
					break;
				case 'A':
					pDisplayer->cameraControl.GO_LEFT = true;
					break;
				case 'S':
					pDisplayer->cameraControl.GO_BACK = true;
					break;
				case 'D':
					pDisplayer->cameraControl.GO_RIGHT = true;
					break;
				case 'Z':
					pDisplayer->cameraControl.GO_DOWN = true;
					break;
				case VK_SPACE:
					pDisplayer->cameraControl.GO_UP = true;
					break;
				case VK_UP:
					pDisplayer->cameraControl.TRUN_UP = true;
					break;
				case VK_DOWN:
					pDisplayer->cameraControl.TRUN_DOWN = true;
					break;
				case VK_LEFT:
					pDisplayer->cameraControl.TRUN_LEFT = true;
					break;
				case VK_RIGHT:
					pDisplayer->cameraControl.TRUN_RIGHT = true;
					break;
			}
			break;
		case WM_KEYUP:
			switch(wParam)
			{
				case 'W':
					pDisplayer->cameraControl.GO_FRONT = false;
					break;
				case 'A':
					pDisplayer->cameraControl.GO_LEFT = false;
					break;
				case 'S':
					pDisplayer->cameraControl.GO_BACK = false;
					break;
				case 'D':
					pDisplayer->cameraControl.GO_RIGHT = false;
					break;
				case 'Z':
					pDisplayer->cameraControl.GO_DOWN = false;
					break;
				case VK_SPACE:
					pDisplayer->cameraControl.GO_UP = false;
					break;
				case VK_UP:
					pDisplayer->cameraControl.TRUN_UP = false;
					break;
				case VK_DOWN:
					pDisplayer->cameraControl.TRUN_DOWN = false;
					break;
				case VK_LEFT:
					pDisplayer->cameraControl.TRUN_LEFT = false;
					break;
				case VK_RIGHT:
					pDisplayer->cameraControl.TRUN_RIGHT = false;
					break;
			}
			break;
	}

	return 0;
}

void UserInput::setDisplayer(Dx11Displayer* pDP)
{
	this->pDisplayer = pDP;
}