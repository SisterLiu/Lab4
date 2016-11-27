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
	matrix View;
	matrix Projection;
	float3 CameraPos;
	float3 LightPos;
}

Texture2D textureFromFile : register(t0);
SamplerState samplerState : register(s0);

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_Target
{
	float ka = 0.5;
	float kd = 0.5;
	float ks = 0.5;
	float3 P = input.PosOld.xyz;
	float3 Lp = LightPos;
	float3 Ep = CameraPos;
	float3 L = normalize(Lp - P);
	float3 N = normalize(input.Normal);
	float3 V = normalize(Ep - P);
	float3 R = 2* dot(L,N)*N-L;

//	float3 cColor = textureFromFile.Sample(samplerState, input.UV).xyz;
	float3 cColor = textureFromFile.Sample(samplerState, input.TextureUV).xyz;
//	float3 cColor = float3(1.0,0,0);

	float diffuse,specular,ambient,iColor;

	ambient = ka;
	diffuse = max(0, dot(L,N))*kd;
	specular = dot(R,V);
	for(int i = 1; i < 5; i++)
	{
		specular = specular * dot(R,V);
	}
	specular = max(0, specular)*ks;

	iColor = diffuse + specular + ambient;
	
	return float4(cColor * iColor,1.0);
}
