#pragma once

#include "Common.h"

typedef enum
{
    RS_CULL_NONE = 0,
    RS_CULL_CCW = 1,
    RS_CULL_CW = 2
}RS_CULL_MODE;

//Create effects constant buffer's structure//
struct cbPerObject
{
	//XMMATRIX  WVP;
	//XMMATRIX World;

	///////////////**************new**************////////////////////
	//These will be used for the pixel shader
	//XMFLOAT4 difColor;
	bool hasTexture;
	///////////////**************new**************////////////////////
};

class MyD3Ddevice
{
public:
	//--------------------------------------------------------------------------------------
	// MyD3Ddevice Variables
	//--------------------------------------------------------------------------------------
	D3D_DRIVER_TYPE         m_driverType;
	D3D_FEATURE_LEVEL       m_featureLevel;
	ID3D11Device*           m_pd3dDevice;
	ID3D11DeviceContext*    m_pImmediateContext;
	IDXGISwapChain*         m_pSwapChain;
	ID3D11RenderTargetView* m_pBackBufferRTV;
    ID3D11Texture2D*        m_pBackBufferDS;
    ID3D11DepthStencilView* m_pBackBufferDSV;

	ID3D11VertexShader*     m_pDefaultVS;
	ID3D11PixelShader*      m_pDefaultPS;
	ID3D11InputLayout*      m_pDefaultLayout;

    ID3D11RasterizerState*  m_pRSCullMode[3];

    D3D11_VIEWPORT vp;

    ID3D11Buffer*           m_pCBPerObjectBuffer;
    cbPerObject             m_cbPerObj;

	MyD3Ddevice();
	~MyD3Ddevice();

	HRESULT InitDevice(HWND g_hWnd);
	void CleanupDevice();
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	void Render();

    std::vector<MyObjLoader> ObjArray; //array of obj models loaded
    void RenderMesh();
};
