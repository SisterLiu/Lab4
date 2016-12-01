#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include <vector>
#include <windows.h>

struct MeshVertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 texture;
	DirectX::XMFLOAT3 normal;
};

class CollisionBlock
{
	public:
		DirectX::XMFLOAT3 center;
		// length to the center, it is R ,not H
		float x;
		float y;
		float z;
		unsigned int type;
		static const unsigned int SPHERE = 0x01;
		static const unsigned int RECT = 0x02;
};

class Mesh
{
	public:
		ID3D11Buffer* pDx11VertexBuffer;
		ID3D11Buffer* pDx11IndexBuffer;
		ID3D11Resource* pTexture;
		ID3D11ShaderResourceView* pDx11TextureView;
		unsigned int numVertex;
		unsigned int numIndex;
		DirectX::XMFLOAT3 posInParent;
		DirectX::XMFLOAT3 posRoot;
		DirectX::XMFLOAT3 angle;
		//std::vector<Mesh> child;
		D3D_PRIMITIVE_TOPOLOGY layout = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		CollisionBlock collision;
		DirectX::XMMATRIX worldMatrixToParent;

		Mesh(ID3D11Device*, ID3D11DeviceContext*, const aiMesh*, float sizeModify = 1.0);
		~Mesh();
		void readTextureFromFile(LPWSTR file);

	private:
		ID3D11Device* pDx11Device;
		ID3D11DeviceContext* pDx11DeviceContext;
};


class Model
{
	public:
		std::vector<Mesh*> meshes;
		Model(ID3D11Device*, ID3D11DeviceContext*, const aiScene*, LPSTR path, float sizeModify = 1.0);
		CollisionBlock collision;
	private:
		aiMesh** ppMesh;
		aiMaterial** ppMaterial;
		void setNode(aiNode*,Mesh**);
		ID3D11Device* pDevice;
		ID3D11DeviceContext* pContext;
};

class Force
{
	public:
		DirectX::XMFLOAT3 direction;
		DirectX::XMFLOAT3 position;
		unsigned int Flag;

		Force() :direction(0,0,0),position(0,0,0),Flag(COLLISION)
		{}

		static const unsigned int USER = 0x01;
		static const unsigned int SUSPEND = 0x02;
		static const unsigned int COLLISION = 0x04;
};

class Motion
{
	public:
		DirectX::XMFLOAT3 speed;
		DirectX::XMFLOAT3 rotation;
		DirectX::XMFLOAT3 gravity;
		float friction;
		float mess;
		bool fixed;
		Motion():speed(0,0,0),rotation(0,0,0),gravity(0,0,0), mess(1), fixed(false), friction(0)
		{}

		std::vector<Force> forces;
};

class Object
{
	public:
		Model* pModel;
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 angle;
		Motion motion;
		unsigned int Flag;
		int id;

		static const unsigned int GROUND = 0x00;
		static const unsigned int WALL = 0x01;
		static const unsigned int BALL = 0x02;

		Object():Flag(BALL)
		{};
};
