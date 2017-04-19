#include <windows.h>
#include "WinProcedure.h"
#include "Assimp/Importer.hpp"
#include "Assimp/Scene.h"
#include "Assimp/Postprocess.h"
#include <ctime>

// private static varible
HWND WinProcedure::hwnd = NULL;
UserInput* WinProcedure::pUserInput = NULL;
Dx11Displayer* WinProcedure::pDisplayer = NULL;
Controller* WinProcedure::pController = NULL;
int WinProcedure::screenX = 0;
int WinProcedure::screenY = 0;
std::vector<Object*> WinProcedure::objects;


//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------

WinProcedure::WinProcedure(HINSTANCE hInstance, int nCmdShow, int screenX, int screenY)
{
	this->screenX = screenX;
	this->screenY = screenY;

	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(nullptr, (LPCTSTR)IDI_APPLICATION);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"CA_LAB_4";
	wcex.hIconSm = LoadIcon(nullptr, (LPCTSTR)IDI_APPLICATION);
	if(!RegisterClassEx(&wcex))
		return;

	// Create window
	RECT rc = {0, 0, screenX, screenY};
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	hwnd = CreateWindow(L"CA_LAB_4", L"Computer Animation Lab 4",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
		nullptr);
	if(hwnd)
		ShowWindow(hwnd, nCmdShow);
}


//---------------------------------------------------------------------------------- -
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WinProcedure::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	LPWSTR text;
	switch(message)
	{
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_MOUSEMOVE:
			pUserInput->mouse(hWnd, message, wParam, lParam);
			break;
		case WM_RBUTTONDOWN:
			pUserInput->mouse(hWnd, message, wParam, lParam);
			break;

		case WM_RBUTTONUP:
			pUserInput->mouse(hWnd, message, wParam, lParam);
			break;

		case WM_KEYUP:
			pUserInput->keyboard(hWnd, message, wParam, lParam);
			break;

		case WM_KEYDOWN:
			pUserInput->keyboard(hWnd, message, wParam, lParam);
			break;
			// Note that this tutorial does not handle resizing (WM_SIZE) requests,
			// so we created the window without the resize border.

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}


void WinProcedure::setUserInput(UserInput* pUI)
{
	this->pUserInput = pUI;
}

void WinProcedure::setDisplayer(Dx11Displayer* pDP)
{
	this->pDisplayer = pDP;
}

void WinProcedure::setController(Controller* pCtrl)
{
	this->pController = pCtrl;
}

HWND WinProcedure::getHWND()
{
	return hwnd;
};

void WinProcedure::initial()
{
	Object* pNewObject = NULL;

	Assimp::Importer importerTest;
	const aiScene* sceneTest = importerTest.ReadFile("./model/bird.3ds", aiProcess_Triangulate);

	
	pNewObject = new Object();
	pNewObject->pModel = new Model(pDisplayer->getDevice(), pDisplayer->getContext(), sceneTest, "./model",0.1);
	pNewObject->pModel->meshes[0]->readTextureFromFile(L"./model/bird.jpg");
	pNewObject->motion.gravity.y = 0;
	pNewObject->angle.x = 0;
	pNewObject->pos.x = 0;
	pNewObject->pos.y = 10;
	pNewObject->pos.z = -10;
	pNewObject->angle.z = 0;
	pNewObject->motion.friction = 0.00;
	pNewObject->motion.speed.x = 0.1;
	objects.push_back(pNewObject);

	for(int i = -1;i<1; i++)
		for(int j = -1; j < 1; j++)
		{
			Object* copyObject = new Object();
			*copyObject = *pNewObject;
			copyObject->pos.x = i*5-50;
			copyObject->pos.z = j * 5;
			copyObject->pos.y = 10 +i+j;
			objects.push_back(copyObject);
		}

	//*/
	sceneTest = importerTest.ReadFile("./model/ground.nff", aiProcess_Triangulate| aiProcess_GenUVCoords | aiProcess_GenNormals);
	pNewObject = new Object();
	pNewObject->pModel = new Model(pDisplayer->getDevice(), pDisplayer->getContext(), sceneTest, "./model", 1);
	pNewObject->pModel->meshes[0]->readTextureFromFile(L"./model/grass.jpg");
	pNewObject->motion.gravity.y = 0;
	pNewObject->motion.fixed = true;
	pNewObject->motion.friction = 0;
	objects.push_back(pNewObject);
	//*/
	SceneBorder border;
	border.xMax = 350;
	border.xMin = -350;
	border.yMax = 200;
	border.yMin = 0;
	border.zMax = 350;
	border.zMin = -350;
	pController->setBorder(border);

	//	Pillar 1
	sceneTest = importerTest.ReadFile("./model/pillar.nff", aiProcess_Triangulate | aiProcess_GenUVCoords | aiProcess_GenNormals);
	pNewObject = new Object();
	pNewObject->pModel = new Model(pDisplayer->getDevice(), pDisplayer->getContext(), sceneTest, "./model", 1);
	pNewObject->pModel->meshes[0]->readTextureFromFile(L"./model/marble.jpg");
	pNewObject->motion.gravity.y = 0;
	pNewObject->pos.y = pNewObject->pModel->collision.y;
	pNewObject->motion.fixed = true;
	pNewObject->motion.friction = 0;
	objects.push_back(pNewObject);

	border.xMax = pNewObject->pos.x + pNewObject->pModel->collision.x;
	border.xMin = pNewObject->pos.x - pNewObject->pModel->collision.x;
	border.yMax = pNewObject->pos.y + pNewObject->pModel->collision.y;
	border.yMin = pNewObject->pos.y - pNewObject->pModel->collision.y;
	border.zMax = pNewObject->pos.z + pNewObject->pModel->collision.z;
	border.zMin = pNewObject->pos.z - pNewObject->pModel->collision.z;
	border.type = SceneBorder::PILLAR;
	pController->setBorder(border);

	//	Pillar 2
	
	Object* copyObject = new Object();
	*copyObject = *pNewObject;
	copyObject->pos.x = 100;
	copyObject->pos.z = 100;
	objects.push_back(copyObject);

	border.xMax = copyObject->pos.x + copyObject->pModel->collision.x;
	border.xMin = copyObject->pos.x - copyObject->pModel->collision.x;
	border.yMax = copyObject->pos.y + copyObject->pModel->collision.y;
	border.yMin = copyObject->pos.y - copyObject->pModel->collision.y;
	border.zMax = copyObject->pos.z + copyObject->pModel->collision.z;
	border.zMin = copyObject->pos.z - copyObject->pModel->collision.z;
	border.type = SceneBorder::PILLAR;
	pController->setBorder(border);

	//	Pillar 3

	copyObject = new Object();
	*copyObject = *pNewObject;
	copyObject->pos.x = 80;
	copyObject->pos.z = -80;
	objects.push_back(copyObject);

	border.xMax = copyObject->pos.x + copyObject->pModel->collision.x;
	border.xMin = copyObject->pos.x - copyObject->pModel->collision.x;
	border.yMax = copyObject->pos.y + copyObject->pModel->collision.y;
	border.yMin = copyObject->pos.y - copyObject->pModel->collision.y;
	border.zMax = copyObject->pos.z + copyObject->pModel->collision.z;
	border.zMin = copyObject->pos.z - copyObject->pModel->collision.z;
	border.type = SceneBorder::PILLAR;
	pController->setBorder(border);
}

Object* WinProcedure::createObjectFromMesh(Mesh* pMesh,
	float x, float y, float z, 
	float angleX, float angleY, float angleZ)
{

	Object* pNewObject = NULL;
	
	pNewObject = new Object();
	//pNewObject->pMesh = pMesh;
	pNewObject->pos.x = x;
	pNewObject->pos.y = y;
	pNewObject->pos.z = z;
	pNewObject->angle.x = angleX;
	pNewObject->angle.y = angleY;
	pNewObject->angle.z = angleZ;

	return pNewObject;
}

int WinProcedure::startLoop()
{
	// Main message loop
	MSG msg = {0};
	while(WM_QUIT != msg.message)
	{
		if(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		//else
		{
			render60();
		}
	}
	return (int)msg.wParam;
}

void WinProcedure::render60()
{
	static ULONGLONG Timer = 0;
	ULONGLONG cur = GetTickCount64();
	if(Timer == 0)
		Timer = cur;
	if(cur - Timer >= 20)
	{
		Timer = cur;
		pController->next(&objects);
		pDisplayer->render(&objects);
	}
}