//--------------------------------------------------------------------------------------
// File: Tutorial04.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
	float4 Pos			: SV_POSITION;
	float4 PosOld		: OLD_POSITION;
	float2 TextureUV	: TUV;
	float3 Normal		: NORMAL;
	float2 UV			: MY_UV;
};

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
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 main(VS_OUTPUT input) : SV_Target
{
	return float4(0,0,0,1.0);
}
