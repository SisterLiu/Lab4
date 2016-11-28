#include "model.h"
#include "Wincodec.h"
#include "WICTextureLoader.h"



//---------------------------------------------------------------------------
//	Class Mesh
//---------------------------------------------------------------------------

Mesh::Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const aiMesh* pAiMesh, float sizeModify)
{
	if(pAiMesh == NULL)
		return;

	worldMatrixToParent = DirectX::XMMatrixIdentity();

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
	if(pTexture) pTexture->Release();
	if(pDx11TextureView) pDx11TextureView->Release();

	CreateWICTextureFromFile(pDx11Device,
		pDx11DeviceContext,
		file,
		&pTexture,
		&pDx11TextureView);
}






//---------------------------------------------------------------------------
//	Class Model
//---------------------------------------------------------------------------

Model::Model(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const aiScene* pScene, LPSTR path,float sizeModify)
{
	this->pDevice = pDevice;
	this->pContext = pContext;
	ppMaterial = pScene->mMaterials;
	ppMesh = pScene->mMeshes;
	Mesh* pNewMesh = NULL;
	bool firstMesh = true;
	for(int i = 0; i < pScene->mNumMeshes; i++)
	{
		pNewMesh = new Mesh(pDevice, pContext, ppMesh[i],sizeModify);
		aiString aiPath;
		aiPath.Clear();
		if(ppMaterial[ppMesh[i]->mMaterialIndex]->GetTextureCount(aiTextureType_DIFFUSE))
			ppMaterial[ppMesh[i]->mMaterialIndex]->GetTexture(aiTextureType_DIFFUSE,0,&aiPath);
		std::string strBuffer = aiPath.C_Str();
		strBuffer = path + ('/'+strBuffer);
		wchar_t realPath[256];
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED,strBuffer.c_str(), -1, realPath,256);
		pNewMesh->readTextureFromFile(realPath);
		meshes.push_back(pNewMesh);
		if(ppMesh[i]->mNumVertices != 0)
		{
			if(firstMesh)
			{
				firstMesh = false;
				collision = pNewMesh->collision;
			}
			else
			{
				float max, min;
				// x
				max = (collision.center.x + collision.x) > (pNewMesh->collision.center.x + pNewMesh->collision.x) ? \
					(collision.center.x + collision.x) : (pNewMesh->collision.center.x + pNewMesh->collision.x);
				min = (collision.center.x - collision.x) < (pNewMesh->collision.center.x - pNewMesh->collision.x) ? \
					(collision.center.x - collision.x) : (pNewMesh->collision.center.x - pNewMesh->collision.x);
				collision.center.x = (max + min) / 2;
				collision.x = max - collision.center.x;
				// y
				max = (collision.center.y + collision.y) > (pNewMesh->collision.center.y + pNewMesh->collision.y) ? \
					(collision.center.y + collision.y) : (pNewMesh->collision.center.y + pNewMesh->collision.y);
				min = (collision.center.y - collision.y) < (pNewMesh->collision.center.y - pNewMesh->collision.y) ? \
					(collision.center.y - collision.y) : (pNewMesh->collision.center.y - pNewMesh->collision.y);
				collision.center.y = (max + min) / 2;
				collision.y = max - collision.center.y;
				// z
				max = (collision.center.z + collision.z) > (pNewMesh->collision.center.z + pNewMesh->collision.z) ? \
					(collision.center.z + collision.z) : (pNewMesh->collision.center.z + pNewMesh->collision.z);
				min = (collision.center.z - collision.z) < (pNewMesh->collision.center.z - pNewMesh->collision.z) ? \
					(collision.center.z - collision.z) : (pNewMesh->collision.center.z - pNewMesh->collision.z);
				collision.center.z = (max + min) / 2;
				collision.z = max - collision.center.z;
			}
		}
	}

}

void Model::setNode(aiNode* pNode, Mesh** ppMesh)
{
	
}

//---------------------------------------------------------------------------
//	Class Object
//---------------------------------------------------------------------------


