#include "dx11Displayer.h"

using namespace DirectX;

#define PI 3.14159265359f
#define R(x) (x * PI / 180.0f)

void Dx11Displayer::render(std::vector<Object*>* pObjects)
{
	//updateCamera();
	updateCamera2(pObjects);

	//-------------------------------------
	// Clear the back buffer
	//-------------------------------------
	pDx11DeviceContext->ClearRenderTargetView(pDx11RenderTargetView, Colors::WhiteSmoke);
	pDx11DeviceContext->ClearDepthStencilView(pDx11DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	pDx11DeviceContext->ClearDepthStencilView(pDx11ShadowView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	
	// Initialize the view matrix
	XMVECTOR Eye = eyePos;
	XMVECTOR LookingAt = eyePos + eyeDirect;
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	constantBuffer.mViewCamera = XMMatrixTranspose(XMMatrixLookAtLH(Eye, LookingAt, Up));

	Eye = XMVectorSet(70.0f, 200.0f, 100.0f, 0.0f);
	LookingAt = XMVectorSet(40.0f, 0.0f, 40.0f, 0.0f);
	constantBuffer.mViewLight = XMMatrixTranspose(XMMatrixLookAtLH(Eye, LookingAt, Up));
	constantBuffer.cameraPos = eyePos;
	constantBuffer.lightPos = Eye;
	//constantBuffer.lightPos = eyePos;
	
	//-------------------------------------------------------------
	//	Shadow
	//-------------------------------------------------------------
	setRenderType(SHADOW);

	// Render
	for(int i = 0; i < pObjects->size(); i++)
	{
		renderObject((*pObjects)[i]);
	}

	//-------------------------------------------------------------
	//	Real
	//-------------------------------------------------------------
	setRenderType(REAL);

	// Render
	for(int i = 0; i < pObjects->size(); i++)
	{
		renderObject((*pObjects)[i]);
	}

	//
	// Present our back buffer to our front buffer
	//
	pDx11SwapChain->Present(0, 0);
}

void Dx11Displayer::renderObject(Object* pObject)
{
	constantBuffer.mWorld = getWorldMatrixFromObject(pObject);
	pDx11DeviceContext->UpdateSubresource(pDx11ConstantBuffer, 0, nullptr, &constantBuffer, 0, 0);
	pDx11DeviceContext->VSSetConstantBuffers(0, 1, &pDx11ConstantBuffer);
	pDx11DeviceContext->PSSetConstantBuffers(0, 1, &pDx11ConstantBuffer);

	UINT stride = sizeof(MeshVertex);
	UINT offset = 0;

	for(int i = 0; i < pObject->pModel->meshes.size(); i++)
	{
		pDx11DeviceContext->IASetVertexBuffers(0, 1, &pObject->pModel->meshes[i]->pDx11VertexBuffer, &stride, &offset);
		pDx11DeviceContext->IASetIndexBuffer(pObject->pModel->meshes[i]->pDx11IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		pDx11DeviceContext->IASetPrimitiveTopology(pObject->pModel->meshes[i]->layout);
		pDx11DeviceContext->PSSetShaderResources(0, 1, &pObject->pModel->meshes[i]->pDx11TextureView);
		pDx11DeviceContext->DrawIndexed(pObject->pModel->meshes[i]->numIndex, 0, 0);
	}
}

XMMATRIX Dx11Displayer::getWorldMatrixFromObject(Object* pObject)
{
	XMMATRIX posM;
	XMFLOAT3 pos = {0,0,0};

	pos.x = pObject->pos.x - pObject->pModel->collision.center.x;
	pos.y = pObject->pos.y - pObject->pModel->collision.center.y;
	pos.z = pObject->pos.z - pObject->pModel->collision.center.z;

	posM.r[0] = XMVectorSet(1.0f, 0.0f, 0.0f, pos.x);
	posM.r[1] = XMVectorSet(0.0f, 1.0f, 0.0f, pos.y);
	posM.r[2] = XMVectorSet(0.0f, 0.0f, 1.0f, pos.z);
	posM.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMMATRIX angleM = XMMatrixIdentity();

	XMFLOAT3 angleEular = pObject->angle;
	XMMATRIX eularM;
	//y rotate
	eularM.r[0] = XMVectorSet(cos(R(angleEular.y)), 0.0f, sin(R(angleEular.y)), 0.0f);
	eularM.r[1] = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	eularM.r[2] = XMVectorSet(-sin(R(angleEular.y)), 0.0f, cos(R(angleEular.y)), 0.0f);
	eularM.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	angleM = angleM * eularM;
	//x rotate
	eularM.r[0] = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	eularM.r[1] = XMVectorSet(0.0f, cos(R(angleEular.x)), -sin(R(angleEular.x)), 0.0f);
	eularM.r[2] = XMVectorSet(0.0f, sin(R(angleEular.x)), cos(R(angleEular.x)), 0.0f);
	eularM.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	angleM = angleM * eularM;
	//z rotate
	eularM.r[0] = XMVectorSet(cos(R(angleEular.z)), -sin(R(angleEular.z)), 0.0f, 0.0f);
	eularM.r[1] = XMVectorSet(sin(R(angleEular.z)), cos(R(angleEular.z)), 0.0f, 0.0f);
	eularM.r[2] = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	eularM.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	angleM = angleM * eularM;

	return posM*angleM;
}

void Dx11Displayer::setRenderType(int type)
{
	if(type == SHADOW)
	{
		pDx11DeviceContext->OMSetRenderTargets(0, NULL, pDx11ShadowView);
		pDx11DeviceContext->VSSetShader(pDx11ShadowVertexShader, nullptr, 0);
		pDx11DeviceContext->IASetInputLayout(pDx11ShadowVertexLayout);
		pDx11DeviceContext->PSSetShader(pDx11ShadowPixelShader, nullptr, 0);
		pDx11DeviceContext->RSSetViewports(1, &shadowViewPort);
	}

	if(type == REAL)
	{
		pDx11DeviceContext->OMSetRenderTargets(1, &pDx11RenderTargetView, pDx11DepthStencilView);
		pDx11DeviceContext->VSSetShader(pDx11VertexShader, nullptr, 0);
		pDx11DeviceContext->IASetInputLayout(pDx11VertexLayout);
		pDx11DeviceContext->PSSetShader(pDx11PixelShader, nullptr, 0);
		pDx11DeviceContext->PSSetShaderResources(1, 1, &pDx11ShadowShaderResourceView);
		pDx11DeviceContext->RSSetViewports(1, &viewPort);
		pDx11DeviceContext->PSSetSamplers(0, 1, &pDx11SamplerState);
		pDx11DeviceContext->PSSetSamplers(1, 1, &pDx11ShadowSamplerState);
	}
}

void Dx11Displayer::updateCamera()
{
	eyePos;
	eyeDirect;
	float speed = 0.3;
	float rotation = 0.02;
	XMVECTOR UP = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR Left = XMVector3Cross(eyeDirect, UP);
	XMVECTOR Front = XMVector3Cross(UP, Left);
	Left = XMVector3Normalize(Left);
	Front = XMVector3Normalize(Front);
	eyeDirect = XMVector3Normalize(eyeDirect);

	if(cameraControl.GO_UP)
	{
		eyePos += speed* UP;
	}
	if(cameraControl.GO_DOWN)
	{
		eyePos -= speed* UP;
	}
	if(cameraControl.GO_LEFT)
	{
		eyePos += speed* Left;
	}
	if(cameraControl.GO_RIGHT)
	{
		eyePos -= speed* Left;
	}
	if(cameraControl.GO_FRONT)
	{
		eyePos += speed* Front;
	}
	if(cameraControl.GO_BACK)
	{
		eyePos -= speed* Front;
	}
	if(cameraControl.TRUN_UP)
	{
		eyeDirect = eyeDirect + XMVector4Normalize(XMVector3Cross(Left,eyeDirect)) *rotation*2;
	}
	if(cameraControl.TRUN_DOWN)
	{
		eyeDirect = eyeDirect - XMVector4Normalize(XMVector3Cross(Left, eyeDirect)) *rotation * 2;
	}
	if(cameraControl.TRUN_LEFT)
	{
		eyeDirect = XMVector4Transform(eyeDirect, XMMatrixTranspose(XMMatrixRotationY(rotation)));
	}
	if(cameraControl.TRUN_RIGHT)
	{
		eyeDirect = XMVector4Transform(eyeDirect, XMMatrixTranspose(XMMatrixRotationY(-rotation)));
	}
}

void Dx11Displayer::updateCamera2(std::vector<Object*>* pObjects)
{
	eyePos;
	eyeDirect;
	float speed = 0.04;
	float rotation = 0.02;
	XMVECTOR UP = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR Left = XMVector3Cross(eyeDirect, UP);
	XMVECTOR Front = XMVector3Cross(UP, Left);
	Left = XMVector3Normalize(Left);
	Front = XMVector3Normalize(Front);
	eyeDirect = XMVector3Normalize(eyeDirect);

	DirectX::XMFLOAT3 userForce = {0,0,0};
	XMVECTOR userForceV = XMVectorSet(0,0,0,0);;

	if(cameraControl.GO_UP)
	{
		userForceV += UP*speed;
	}
	if(cameraControl.GO_DOWN)
	{
		userForceV -= UP*speed;
	}
	if(cameraControl.GO_LEFT)
	{
		userForceV += speed* Left;
	}
	if(cameraControl.GO_RIGHT)
	{
		userForceV -= speed* Left;
	}
	if(cameraControl.GO_FRONT)
	{
		userForceV += speed* Front;
	}
	if(cameraControl.GO_BACK)
	{
		userForceV -= speed* Front;
	}
	if(cameraControl.TRUN_UP)
	{
		eyeDirect = eyeDirect + XMVector4Normalize(XMVector3Cross(Left, eyeDirect)) *rotation * 2;
	}
	if(cameraControl.TRUN_DOWN)
	{
		eyeDirect = eyeDirect - XMVector4Normalize(XMVector3Cross(Left, eyeDirect)) *rotation * 2;
	}
	if(cameraControl.TRUN_LEFT)
	{
		eyeDirect = XMVector4Transform(eyeDirect, XMMatrixTranspose(XMMatrixRotationY(rotation)));
	}
	if(cameraControl.TRUN_RIGHT)
	{
		eyeDirect = XMVector4Transform(eyeDirect, XMMatrixTranspose(XMMatrixRotationY(-rotation)));
	}
	if(cameraControl.CAMERA_NEAR)
	{
		cameraDistance -= 1;
	}
	if(cameraControl.CAMERA_FAR)
	{
		cameraDistance += 1;
	}

	XMStoreFloat3(&userForce,userForceV);
	Force force;
	force.direction = userForce;
	force.Flag = Force::USER;

	for(int i = 0; i < pObjects->size(); i++)
	{
		if(!(*pObjects)[i]->motion.fixed)
			(*pObjects)[i]->motion.forces.push_back(force);
	}


	XMFLOAT3 center = {0,0,0};
	int birdCount = 0;
	for(int i = 0; i < pObjects->size(); i++)
	{
		if((*pObjects)[i]->motion.fixed)
			continue;
		center.x += (*pObjects)[i]->pos.x;
		center.y += (*pObjects)[i]->pos.y;
		center.z += (*pObjects)[i]->pos.z;
		birdCount++;
	}
	center.x /= birdCount;
	center.y /= birdCount;
	center.z /= birdCount;

	eyeDirect = XMVector3Normalize(eyeDirect);
	eyePos = XMLoadFloat3(&center) - cameraDistance * eyeDirect;
}

Dx11Displayer::Dx11Displayer(HWND hwnd)
{
	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect(hwnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;
	int shadowSizeModify = 2;

	//-------------------------------------------------------------------------
	//	Dx11 Device and Swap Chain
	//-------------------------------------------------------------------------
	// create the device describe
	DXGI_SWAP_CHAIN_DESC swapChainDescribe;
	ZeroMemory(&swapChainDescribe, sizeof(swapChainDescribe));
	swapChainDescribe.BufferCount = 1;
	swapChainDescribe.BufferDesc.Width = width;
	swapChainDescribe.BufferDesc.Height = height;
	swapChainDescribe.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDescribe.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDescribe.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDescribe.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDescribe.OutputWindow = hwnd;
	swapChainDescribe.SampleDesc.Count = 1;
	swapChainDescribe.SampleDesc.Quality = 0;
	swapChainDescribe.Windowed = TRUE;

	D3D_FEATURE_LEVEL  FeatureLevelsRequested = D3D_FEATURE_LEVEL_11_0;
	UINT               numLevelsRequested = 1;
	D3D_FEATURE_LEVEL  FeatureLevelsSupported;

	//	create & set the device
	if(FAILED(hr = D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		0,
		&FeatureLevelsRequested,
		numLevelsRequested,
		D3D11_SDK_VERSION,
		&swapChainDescribe,
		&pDx11SwapChain,
		&pDx11Device,
		&FeatureLevelsSupported,
		&pDx11DeviceContext)))
	{
		return;
	}
	//-------------------------------------------------------------------------
	//	Render Target View
	//-------------------------------------------------------------------------
	// Create a render target view
	ID3D11Texture2D* pBackBuffer = nullptr;
	hr = pDx11SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
	if(FAILED(hr))
		return;

	hr = pDx11Device->CreateRenderTargetView(pBackBuffer, nullptr, &pDx11RenderTargetView);
	pBackBuffer->Release();
	if(FAILED(hr))
		return;

	//-------------------------------------------------------------------------
	//	Shadow
	//-------------------------------------------------------------------------
	//	Shadow Texture2D Describe
	D3D11_TEXTURE2D_DESC descShadow;
	ZeroMemory(&descShadow, sizeof(descShadow));
	descShadow.Width = width * shadowSizeModify;
	descShadow.Height = height * shadowSizeModify;
	descShadow.MipLevels = 1;
	descShadow.ArraySize = 1;
	descShadow.Format = DXGI_FORMAT_R24G8_TYPELESS;
	descShadow.SampleDesc.Count = 1;
	descShadow.SampleDesc.Quality = 0;
	descShadow.Usage = D3D11_USAGE_DEFAULT;
	descShadow.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	descShadow.CPUAccessFlags = 0;
	descShadow.MiscFlags = 0;
	hr = pDx11Device->CreateTexture2D(&descShadow, nullptr, &pDx11Shadow);
	if(FAILED(hr))
		return;

	//	Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descShadowView;
	ZeroMemory(&descShadowView, sizeof(descShadowView));
	descShadowView.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descShadowView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descShadowView.Texture2D.MipSlice = 0;
	hr = pDx11Device->CreateDepthStencilView(pDx11Shadow, &descShadowView, &pDx11ShadowView);
	if(FAILED(hr))
		return;

	//	Create Shader Resouce view
	D3D11_SHADER_RESOURCE_VIEW_DESC descShadowShaderResource;
	ZeroMemory(&descShadowShaderResource, sizeof(descShadowShaderResource));
	descShadowShaderResource.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	descShadowShaderResource.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	descShadowShaderResource.Texture2D.MostDetailedMip = 0;
	descShadowShaderResource.Texture2D.MipLevels = 1;
	hr = pDx11Device->CreateShaderResourceView(pDx11Shadow, &descShadowShaderResource, &pDx11ShadowShaderResourceView);
	if(FAILED(hr))
		return;


	//-------------------------------------------------------------------------
	//	Depth Stencil
	//-------------------------------------------------------------------------
	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = pDx11Device->CreateTexture2D(&descDepth, nullptr, &pDx11DepthStencil);
	if(FAILED(hr))
		return;

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = pDx11Device->CreateDepthStencilView(pDx11DepthStencil, &descDSV, &pDx11DepthStencilView);
	if(FAILED(hr))
		return;

	//-------------------------------------------------------------------------
	//	Set Render Target View & Depth Stencil View
	//-------------------------------------------------------------------------
	pDx11DeviceContext->OMSetRenderTargets(1, &pDx11RenderTargetView, pDx11DepthStencilView);


	//-------------------------------------------------------------------------
	//	Set View Port
	//-------------------------------------------------------------------------
	viewPort.Width = width;
	viewPort.Height = height;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	pDx11DeviceContext->RSSetViewports(1, &viewPort);

	//-------------------------------------------------------------------------
	//	Set Shadow View Port
	//-------------------------------------------------------------------------
	D3D11_VIEWPORT vp;
	shadowViewPort.Width = width * shadowSizeModify;
	shadowViewPort.Height = height * shadowSizeModify;
	shadowViewPort.MinDepth = 0.0f;
	shadowViewPort.MaxDepth = 1.0f;
	shadowViewPort.TopLeftX = 0;
	shadowViewPort.TopLeftY = 0;


	//-------------------------------------------------------------------------
	//	Vertex Shader
	//-------------------------------------------------------------------------
	ID3DBlob* pVSBlob = nullptr;

	// Compile the vertex shader
	hr = CompileShaderFromFile(L"VertexShader.hlsl", "VS", "vs_4_0", &pVSBlob);
	if(FAILED(hr))
	{
		MessageBox(nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return;
	}

	// Create the vertex shader
	hr = pDx11Device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &pDx11VertexShader);
	if(FAILED(hr))
	{
		pVSBlob->Release();
		return;
	}

	// Set the vertex shader
	pDx11DeviceContext->VSSetShader(pDx11VertexShader, nullptr, 0);

	//-------------------------------------------------------------------------
	//	Input Layout base on Vertex Shader
	//-------------------------------------------------------------------------
	// Create the input layout
	hr = pDx11Device->CreateInputLayout(layout, 3, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &pDx11VertexLayout);
	pVSBlob->Release();
	if(FAILED(hr))
		return;

	// Set the input layout
	pDx11DeviceContext->IASetInputLayout(pDx11VertexLayout);

	//-------------------------------------------------------------------------
	//	Pixel Shader
	//-------------------------------------------------------------------------
	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
	hr = CompileShaderFromFile(L"PixelShader.hlsl", "PS", "ps_4_0", &pPSBlob);
	if(FAILED(hr))
	{
		MessageBox(nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return;
	}

	// Create the pixel shader
	hr = pDx11Device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &pDx11PixelShader);
	pPSBlob->Release();
	if(FAILED(hr))
		return;

	// Set the pixel shader
	pDx11DeviceContext->PSSetShader(pDx11PixelShader, nullptr, 0);

	//-------------------------------------------------------------------------
	//	Shadow Vertex Shader
	//-------------------------------------------------------------------------

	// Compile the Shadow vertex shader
	hr = CompileShaderFromFile(L"ShadowVertexShader.hlsl", "main", "vs_4_0", &pVSBlob);
	if(FAILED(hr))
	{
		MessageBox(nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return;
	}

	// Create the Shadow vertex shader
	hr = pDx11Device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &pDx11ShadowVertexShader);
	if(FAILED(hr))
	{
		pVSBlob->Release();
		return;
	}

	//-------------------------------------------------------------------------
	//	Input Layout base on Shadow Vertex Shader
	//-------------------------------------------------------------------------
	// Create the input layout
	hr = pDx11Device->CreateInputLayout(layout, 3, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &pDx11ShadowVertexLayout);
	pVSBlob->Release();
	if(FAILED(hr))
		return;

	//-------------------------------------------------------------------------
	//	Shadow Pixel Shader
	//-------------------------------------------------------------------------
	// Compile the Shadow pixel shader
	hr = CompileShaderFromFile(L"ShadowPixelShader.hlsl", "main", "ps_4_0", &pPSBlob);
	if(FAILED(hr))
	{
		MessageBox(nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return;
	}

	// Create the Shadow pixel shader
	hr = pDx11Device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &pDx11ShadowPixelShader);
	pPSBlob->Release();
	if(FAILED(hr))
		return;

	//-------------------------------------------------------------------------
	//	Vertex Texture Sampler
	//-------------------------------------------------------------------------
	D3D11_SAMPLER_DESC samplerDescribe;
	ZeroMemory(&samplerDescribe,sizeof(samplerDescribe));
	samplerDescribe.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDescribe.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescribe.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescribe.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescribe.BorderColor[0] = 0;
	samplerDescribe.BorderColor[1] = 0;
	samplerDescribe.BorderColor[2] = 0;
	samplerDescribe.BorderColor[3] = 1;
	samplerDescribe.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDescribe.MaxLOD = 4;
	samplerDescribe.MinLOD = 0;

	//	Create SamplerState
	pDx11Device->CreateSamplerState(&samplerDescribe,&pDx11SamplerState);

	//	Set SamplerState
	pDx11DeviceContext->PSSetSamplers(0, 1, &pDx11SamplerState);

	//-------------------------------------------------------------------------
	//	Shadow Texture Sampler
	//-------------------------------------------------------------------------
	D3D11_SAMPLER_DESC shadowSamplerDescribe;
	ZeroMemory(&shadowSamplerDescribe, sizeof(shadowSamplerDescribe));
	shadowSamplerDescribe.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	shadowSamplerDescribe.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSamplerDescribe.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSamplerDescribe.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSamplerDescribe.BorderColor[0] = 1;
	shadowSamplerDescribe.BorderColor[1] = 1;
	shadowSamplerDescribe.BorderColor[2] = 1;
	shadowSamplerDescribe.BorderColor[3] = 1;
	shadowSamplerDescribe.ComparisonFunc = D3D11_COMPARISON_NEVER;
	shadowSamplerDescribe.MaxLOD = 4;
	shadowSamplerDescribe.MinLOD = 0;

	//	Create SamplerState
	pDx11Device->CreateSamplerState(&shadowSamplerDescribe, &pDx11ShadowSamplerState);

	//-------------------------------------------------------------------------
	//	Constant Buffer
	//-------------------------------------------------------------------------
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	// Create the constant buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = pDx11Device->CreateBuffer(&bd, nullptr, &pDx11ConstantBuffer);
	if(FAILED(hr))
		return;

	// Initialize the world matrix
	constantBuffer.mWorld = XMMatrixIdentity();

	// Initialize the view matrix
	XMVECTOR Eye = XMVectorSet(40.0f, 5.0f, -10.0f, 0.0f);
	XMVECTOR LookingAt = XMVectorSet(0.0f, -5.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	constantBuffer.mViewCamera = XMMatrixTranspose(XMMatrixLookAtLH(Eye, LookingAt, Up));

	// Initialize the projection matrix
	constantBuffer.mProjection = XMMatrixTranspose(XMMatrixPerspectiveFovLH(XM_PIDIV2, width / (FLOAT)height, 0.01f, 1000.0f));

	//	
	eyeDirect = LookingAt - Eye;
	eyeDirect = XMVector4Normalize(eyeDirect);
	eyePos = Eye;
	cameraDistance = 40;

	//	cameraControl
	cameraControl.GO_UP = false;
	cameraControl.GO_DOWN = false;
	cameraControl.GO_LEFT = false;
	cameraControl.GO_RIGHT = false;
	cameraControl.GO_FRONT = false;
	cameraControl.GO_BACK = false;
	cameraControl.TRUN_UP = false;
	cameraControl.TRUN_DOWN = false;
	cameraControl.TRUN_LEFT = false;
	cameraControl.TRUN_RIGHT = false;
	cameraControl.CAMERA_FAR = false;
	cameraControl.CAMERA_NEAR = false;
}

Dx11Displayer::~Dx11Displayer()
{
	if(pDx11Device)						pDx11Device->Release();
	if(pDx11DeviceContext)				pDx11DeviceContext->Release();

	if(pDx11SwapChain)					pDx11SwapChain->Release();
	if(pDx11RenderTargetView)			pDx11RenderTargetView->Release();

	if(pDx11DepthStencil)				pDx11DepthStencil->Release();
	if(pDx11DepthStencilView)			pDx11DepthStencilView->Release();

	if(pDx11Shadow)						pDx11Shadow->Release();
	if(pDx11ShadowView)					pDx11ShadowView->Release();
	if(pDx11ShadowShaderResourceView)	pDx11ShadowShaderResourceView->Release();
	if(pDx11ShadowSamplerState)			pDx11ShadowSamplerState->Release();

	if(pDx11ShadowVertexShader)			pDx11ShadowVertexShader->Release();
	if(pDx11ShadowPixelShader)			pDx11ShadowPixelShader->Release();
	if(pDx11ShadowVertexLayout)			pDx11ShadowVertexLayout->Release();

	if(pDx11VertexShader)				pDx11VertexShader->Release();
	if(pDx11PixelShader)				pDx11PixelShader->Release();
	if(pDx11VertexLayout)				pDx11VertexLayout->Release();


	if(pDx11ConstantBuffer)				pDx11ConstantBuffer->Release();
	if(pDx11SamplerState)				pDx11SamplerState->Release();
}

//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DCompile
//
// With VS 11, we could load up prebuilt .cso files instead...
//--------------------------------------------------------------------------------------
HRESULT Dx11Displayer::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;

	// Disable optimizations to further improve shader debugging
	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob* pErrorBlob = nullptr;
	hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
	if(FAILED(hr))
	{
		if(pErrorBlob)
		{
			OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
			pErrorBlob->Release();
		}
		return hr;
	}
	if(pErrorBlob) pErrorBlob->Release();

	return S_OK;
}
