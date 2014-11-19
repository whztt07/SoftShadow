#pragma once

#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "./DirextXTK/Inc/WICTextureLoader.h"
#include "./DirextXTK/Inc/DDSTextureLoader.h"

#include <vector>
#include <fstream>
#include <istream>
#include <sstream>

using namespace DirectX;

#define MAX_PATH 260
#define PI 3.141592654f

typedef unsigned int uint32; 

typedef XMFLOAT2 float2;
typedef XMFLOAT3 float3;
typedef XMFLOAT4 float4;

struct Vertex	//mesh Vertex Structure
{
	Vertex(){}
	Vertex(float x, float y, float z,
		float u, float v,
		float nx, float ny, float nz)
		: pos(x,y,z), texCoord(u, v), normal(nx, ny, nz){}

	float3 pos;
	float2 texCoord;
	float3 normal;
};