#pragma once
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "model.h"

struct ConstantBuffer
{
	DirectX::XMMATRIX mWorld;
	DirectX::XMMATRIX mViewCamera;
	DirectX::XMMATRIX mViewLight;
	DirectX::XMMATRIX mProjection;
	DirectX::XMVECTOR cameraPos;
	DirectX::XMVECTOR lightPos;
};

struct CameraControl
{
	bool GO_UP;
	bool GO_LEFT;
	bool GO_RIGHT;
	bool GO_DOWN;
	bool GO_FRONT;
	bool GO_BACK;
	bool TRUN_LEFT;
	bool TRUN_RIGHT;
	bool TRUN_DOWN;
	bool TRUN_UP;
};

typedef class Dx11Displayer
{
	public:
		void render(std::vector<Object*>*);// render a frame
		void renderObject(Object*);

		Dx11Displayer(HWND);
		~Dx11Displayer();

		DirectX::XMVECTOR		eyePos;
		DirectX::XMVECTOR		eyeDirect;
		CameraControl			cameraControl;

		ID3D11Device*			getDevice()	{return pDx11Device;}
		ID3D11DeviceContext*	getContext(){return pDx11DeviceContext;}

	private:
		ID3D11Device*				pDx11Device;
		ID3D11DeviceContext*		pDx11DeviceContext;

		IDXGISwapChain*				pDx11SwapChain;
		ID3D11RenderTargetView*		pDx11RenderTargetView;

		ID3D11Texture2D*			pDx11DepthStencil;
		ID3D11DepthStencilView*		pDx11DepthStencilView;

		ID3D11Texture2D*			pDx11Shadow;
		ID3D11DepthStencilView*		pDx11ShadowView;
		ID3D11ShaderResourceView*	pDx11ShadowShaderResourceView;

		ID3D11VertexShader*			pDx11ShadowVertexShader;
		ID3D11PixelShader*			pDx11ShadowPixelShader;
		ID3D11InputLayout*			pDx11ShadowVertexLayout;

		ID3D11VertexShader*			pDx11VertexShader;
		ID3D11PixelShader*			pDx11PixelShader;
		ID3D11InputLayout*			pDx11VertexLayout;


		ID3D11Buffer*				pDx11ConstantBuffer;
		ID3D11SamplerState*			pDx11SamplerState;
		ConstantBuffer				constantBuffer;

		D3D11_VIEWPORT				viewPort;
		D3D11_VIEWPORT				shadowViewPort;

		HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
		void updateCamera();
		void updateCamera2(Object*);
		void setRenderType(int type);

		static const int SHADOW		= 0x01;
		static const int REAL		= 0x02;

		DirectX::XMMATRIX getWorldMatrixFromObject(Object*);

		D3D11_INPUT_ELEMENT_DESC layout[3] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};
}DX11DISPLAYER, *PDX11DISPLAYER;

