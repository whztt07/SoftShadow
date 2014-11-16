//--------------------------------------------------------------------------------------
// File: Tutorial02.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "common.fxh"

cbuffer cbPerObject
{
	//float4x4 WVP;
    //float4x4 World;

	//float4 difColor;
	bool hasTexture;
};

Texture2D ObjTexture;

struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float4 worldPos : POSITION;
	float2 TexCoord : TEXCOORD;
	float3 normal : NORMAL;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(float4 inPos : POSITION, float2 inTexCoord : TEXCOORD, float3 normal : NORMAL)
{
    VS_OUTPUT output;

    output.Pos = inPos;//mul(inPos, WVP);
	//output.worldPos = mul(inPos, World);

	output.normal = normal;//mul(normal, World);

    output.TexCoord = inTexCoord;

    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT input ) : SV_Target
{
    if(hasTexture == true)
		return ObjTexture.Sample( LinearSampler, input.TexCoord );

    return float4( 1.0f, 1.0f, 1.0f, 1.0f );    // Yellow, with Alpha = 1
}
