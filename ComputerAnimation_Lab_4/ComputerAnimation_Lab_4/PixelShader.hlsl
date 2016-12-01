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

Texture2D textureFromFile : register(t0);
Texture2D textureShadow	  :	register(t1);
SamplerState samplerState : register(s0);
SamplerState shadowSamplerState : register(s1);

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

	float4 PinLight = input.PosOld;
	PinLight = mul(PinLight, ViewLight);
	PinLight = mul(PinLight, Projection);
	PinLight = PinLight / PinLight.w;

	float2 texUV = input.TextureUV.xy;
	if(input.TextureUV.x < 0.0001 && input.TextureUV.y < 0.0001)
	{
		texUV.x = input.UV.x/40;
		texUV.y = input.UV.y/40;
	}

	float3 cColor = textureFromFile.Sample(samplerState, texUV).xyz;

	float diffuse,specular,ambient,iColor;

	ambient = ka;
	diffuse = max(0, dot(L,N))*kd;
	specular = dot(R,V);
	for(int i = 1; i < 5; i++)
	{
		specular = specular * dot(R,V);
	}
	specular = max(0, specular)*ks;
	
	PinLight.x = PinLight.x / 2 + 0.5;
	PinLight.y = -PinLight.y / 2 + 0.5;

	if(PinLight.z > textureShadow.Sample(shadowSamplerState, PinLight.xy).x+0.000001)
	{
		iColor = ambient;
	}
	else
	{
		iColor = diffuse + specular + ambient;
	}
	
	return float4(cColor * iColor,1.0);
}
