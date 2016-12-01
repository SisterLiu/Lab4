//--------------------------------------------------------------------------------------
// File: Tutorial04.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register(b0)
{
	matrix World;
	matrix ViewCamera;
	matrix ViewLight;
	matrix Projection;
	float3 CameraPos;
	float3 LightPos;
}

//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
	float4 Pos			: SV_POSITION;
	float4 PosOld		: OLD_POSITION;
	float2 TextureUV	: TUV;
	float3 Normal		: NORMAL;
	float2 UV			: MY_UV;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(float4 Pos : POSITION, float2 Tex : TEXTURE, float3 Normal : NORMAL)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Pos = mul(Pos, World);
	output.PosOld = output.Pos;
	output.Pos = mul(output.Pos, ViewCamera);
	output.Pos = mul(output.Pos, Projection);
	output.TextureUV = Tex;
	output.Normal = mul(Normal, World);
	output.UV.x = Pos.x;
	output.UV.y = Pos.z + Pos.y;
	return output;
}

