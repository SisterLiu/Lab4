#include "model.h"
#include "Wincodec.h"
#include "WICTextureLoader.h"

//---------------------------------------------------------------------------
//	Class Mesh
//---------------------------------------------------------------------------

Mesh::Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const aiMesh* pAiMesh)
{
	float sizeModify = 1;
	numVertex = pAiMesh->mNumVertices;
	numIndex = pAiMesh->mNumFaces * 3;

	this->pDx11Device = pDevice;
	this->pDx11DeviceContext = pDeviceContext;

	//	Set the vertex & index buffer
	MeshVertex* pVertexs = new MeshVertex[numVertex];
	unsigned int* pIndexs = new unsigned int[numIndex];
	
	//------------------------------------------------------------------
	//	Vertex & Collision Block
	//------------------------------------------------------------------
	float xMax, xMin, yMax, yMin, zMax, zMin;
	xMax = yMax = zMax = FLT_MIN;
	xMin = yMin = zMin = FLT_MAX;

	//	Set vertex position
	for(int i = 0; i < numVertex; i++)
	{
		pVertexs[i].pos.x = pAiMesh->mVertices[i].x*sizeModify;
		pVertexs[i].pos.y = pAiMesh->mVertices[i].y*sizeModify;
		pVertexs[i].pos.z = pAiMesh->mVertices[i].z*sizeModify;
		if(pVertexs[i].pos.x < xMin)
			xMin = pVertexs[i].pos.x;
		if(pVertexs[i].pos.y < yMin)
			yMin = pVertexs[i].pos.y;
		if(pVertexs[i].pos.z < zMin)
			zMin = pVertexs[i].pos.z;
		if(pVertexs[i].pos.x > xMax)
			xMax = pVertexs[i].pos.x;
		if(pVertexs[i].pos.y > yMax)
			yMax = pVertexs[i].pos.y;
		if(pVertexs[i].pos.z > zMax)
			zMax = pVertexs[i].pos.z;
	}

	//	Set Collision
	collision.center.x = (xMax + xMin) / 2;
	collision.center.y = (yMax + yMin) / 2;
	collision.center.z = (zMax + zMin) / 2;
	collision.x = xMax - collision.center.x;
	collision.y = yMax - collision.center.y;
	collision.z = zMax - collision.center.z;
	collision.type = CollisionBlock::SPHERE;

	//	Set vertex normal
	if(pAiMesh->HasNormals())
		for(int i = 0; i < numVertex; i++)
		{
			pVertexs[i].normal.x = pAiMesh->mNormals[i].x;
			pVertexs[i].normal.y = pAiMesh->mNormals[i].y;
			pVertexs[i].normal.z = pAiMesh->mNormals[i].z;
		}

	//	Set vertex texture coordinate
	if(pAiMesh->HasTextureCoords(0))
	for(int i = 0; i < numVertex; i++)
	{
		pVertexs[i].texture.x = pAiMesh->mTextureCoords[0][i].x;
		pVertexs[i].texture.y = 1-pAiMesh->mTextureCoords[0][i].y;
	}

	//------------------------------------------------------------------
	//	Index
	//------------------------------------------------------------------
	//	Set Index
	for(int i = 0; i < pAiMesh->mNumFaces; i++)
	{
		pIndexs[i * 3 + 0] = pAiMesh->mFaces[i].mIndices[0];
		pIndexs[i * 3 + 1] = pAiMesh->mFaces[i].mIndices[1];
		pIndexs[i * 3 + 2] = pAiMesh->mFaces[i].mIndices[2];
	}

	//------------------------------------------------------------------
	//	Create Vertex Buffer
	//------------------------------------------------------------------
	//	Set vertex buffer describe
	D3D11_BUFFER_DESC vertexBufferDescribe;
	ZeroMemory(&vertexBufferDescribe,sizeof(vertexBufferDescribe));
	vertexBufferDescribe.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDescribe.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDescribe.ByteWidth = sizeof(MeshVertex) * numVertex;
	vertexBufferDescribe.CPUAccessFlags = 0;
	
	//	Set vertex buffer data
	D3D11_SUBRESOURCE_DATA vertexBufferData;
	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = pVertexs;
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;

	//	Create Vertex Buffer
	pDx11Device->CreateBuffer(&vertexBufferDescribe,&vertexBufferData,&pDx11VertexBuffer);

	//------------------------------------------------------------------
	//	Create Index Buffer
	//------------------------------------------------------------------
	//	Set index buffer describe
	D3D11_BUFFER_DESC indexBufferDescribe;
	ZeroMemory(&indexBufferDescribe, sizeof(indexBufferDescribe));
	indexBufferDescribe.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDescribe.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDescribe.ByteWidth = sizeof(unsigned int) * numIndex;
	indexBufferDescribe.CPUAccessFlags = 0;

	//	Set index buffer data
	D3D11_SUBRESOURCE_DATA indexBufferData;
	ZeroMemory(&indexBufferData, sizeof(indexBufferData));
	indexBufferData.pSysMem = pIndexs;
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;

	//	Create Vertex Buffer
	pDx11Device->CreateBuffer(&indexBufferDescribe, &indexBufferData, &pDx11IndexBuffer);

	//------------------------------------------------------------------
	//	Release vertex & index buffer
	//------------------------------------------------------------------
	delete[] pVertexs;
	delete[] pIndexs;

}

Mesh::~Mesh()
{
	if(pDx11VertexBuffer) pDx11VertexBuffer->Release();
	if(pDx11IndexBuffer) pDx11IndexBuffer->Release();
	if(pTexture) pTexture->Release();
	if(pDx11TextureView) pDx11TextureView->Release();
}

void Mesh::readTextureFromFile(LPWSTR file)
{
	CreateWICTextureFromFile(pDx11Device,
		pDx11DeviceContext,
		file,
		&pTexture,
		&pDx11TextureView);
}






//---------------------------------------------------------------------------
//	Class Model
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
//	Class Object
//---------------------------------------------------------------------------


