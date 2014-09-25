#pragma once

#include "Common.h"

class MyD3Ddevice
{
public:
	//--------------------------------------------------------------------------------------
	// MyD3Ddevice Variables
	//--------------------------------------------------------------------------------------
	D3D_DRIVER_TYPE         g_driverType;
	D3D_FEATURE_LEVEL       g_featureLevel;
	ID3D11Device*           g_pd3dDevice;
	ID3D11DeviceContext*    g_pImmediateContext;
	IDXGISwapChain*         g_pSwapChain;
	ID3D11RenderTargetView* g_pRenderTargetView;
	ID3D11VertexShader*     g_pVertexShader;
	ID3D11PixelShader*      g_pPixelShader;
	ID3D11InputLayout*      g_pVertexLayout;
	ID3D11Buffer*           g_pVertexBuffer;

	MyD3Ddevice();
	~MyD3Ddevice();

	HRESULT InitDevice(HWND g_hWnd);
	void CleanupDevice();
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	void Render();
};
