#include "MyD3Ddevice.h"

//--------------------------------------------------------------------------------------
// MyD3Ddevice Constructor
//--------------------------------------------------------------------------------------
MyD3Ddevice::MyD3Ddevice()
{
    m_driverType = D3D_DRIVER_TYPE_NULL;
    m_featureLevel = D3D_FEATURE_LEVEL_11_0;
    m_pd3dDevice = NULL;
    m_pImmediateContext = NULL;
    m_pSwapChain = NULL;
    m_pBackBufferRTV = NULL;
    m_pDefaultVS = NULL;
    m_pDefaultPS = NULL;
    m_pDefaultLayout = NULL;
    m_pBackBufferDS = NULL;
    m_pBackBufferDSV = NULL;
    m_pRSCullMode[0] = NULL;
    m_pRSCullMode[1] = NULL;
    m_pRSCullMode[2] = NULL;

    m_pCBPerObjectBuffer = NULL;

    ObjArray.clear();
}

//--------------------------------------------------------------------------------------
// MyD3Ddevice Deconstructor
//--------------------------------------------------------------------------------------
MyD3Ddevice::~MyD3Ddevice()
{
	//Todo: add code here
}

//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DX11
//--------------------------------------------------------------------------------------
HRESULT MyD3Ddevice::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob;
	hr = D3DCompileFromFile(szFileName, NULL, NULL, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
	if (FAILED(hr))
	{
		if (pErrorBlob != NULL)
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
		if (pErrorBlob) pErrorBlob->Release();
		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
HRESULT MyD3Ddevice::InitDevice(HWND g_hWnd)
{
	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect(g_hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = g_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		m_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL, m_driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &m_pSwapChain, &m_pd3dDevice, &m_featureLevel, &m_pImmediateContext);
		if (SUCCEEDED(hr))
			break;
	}
	if (FAILED(hr))
		return hr;

	// Create backbuffer render target view
	ID3D11Texture2D* pBackBuffer = NULL;
	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(hr))
		return hr;

	hr = m_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pBackBufferRTV);
	pBackBuffer->Release();
	if (FAILED(hr))
		return hr;

    //Describe our Depth/Stencil Buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width     = width;
	depthStencilDesc.Height    = height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count   = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage          = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0; 
	depthStencilDesc.MiscFlags      = 0;

	//Create the default Depth/Stencil View
	m_pd3dDevice->CreateTexture2D(&depthStencilDesc, NULL, &m_pBackBufferDS);
	m_pd3dDevice->CreateDepthStencilView(m_pBackBufferDS, NULL, &m_pBackBufferDSV);

	// Setup the default viewport
	vp.Width = (FLOAT)width;
	vp.Height = (FLOAT)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	// Compile the default vertex shader
	ID3DBlob* pVSBlob = NULL;
	hr = CompileShaderFromFile(L"MyD3D11.fx", "VS", "vs_4_0", &pVSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the default vertex shader
	hr = m_pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &m_pDefaultVS);
	if (FAILED(hr))
	{
		pVSBlob->Release();
		return hr;
	}

    // Compile the default pixel shader
	ID3DBlob* pPSBlob = NULL;
	hr = CompileShaderFromFile(L"MyD3D11.fx", "PS", "ps_4_0", &pPSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the default pixel shader
	hr = m_pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &m_pDefaultPS);
	pPSBlob->Release();
	if (FAILED(hr))
		return hr;

	// Define the default input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
    {
	    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
	    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
	    { "NORMAL",	 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
	UINT numElements = ARRAYSIZE(layout);

	// Create the default input layout
	hr = m_pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &m_pDefaultLayout);
	pVSBlob->Release();
	if (FAILED(hr))
		return hr;

    //Create the buffer to send to the cbuffer in effect file
	D3D11_BUFFER_DESC cbbd;	
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));
	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = sizeof(cbPerObject);
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;
	hr = m_pd3dDevice->CreateBuffer(&cbbd, NULL, &m_pCBPerObjectBuffer);

    // Create raster state
    D3D11_RASTERIZER_DESC cmdesc;
	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_BACK;
	cmdesc.FrontCounterClockwise = true;
    hr = m_pd3dDevice->CreateRasterizerState(&cmdesc, &m_pRSCullMode[RS_CULL_CCW]);

	cmdesc.FrontCounterClockwise = false;
	hr = m_pd3dDevice->CreateRasterizerState(&cmdesc, &m_pRSCullMode[RS_CULL_CW]);

	cmdesc.CullMode = D3D11_CULL_NONE;
	hr = m_pd3dDevice->CreateRasterizerState(&cmdesc, &m_pRSCullMode[RS_CULL_NONE]);

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
void MyD3Ddevice::CleanupDevice()
{
    for(uint32 i=0; i<ObjArray.size(); i++)
        ObjArray[i].Destroy();
    ObjArray.clear();

	if (m_pImmediateContext) m_pImmediateContext->ClearState();

	if (m_pDefaultLayout) m_pDefaultLayout->Release();
	if (m_pDefaultVS) m_pDefaultVS->Release();
	if (m_pDefaultPS) m_pDefaultPS->Release();
	if (m_pBackBufferRTV) m_pBackBufferRTV->Release();
    if (m_pBackBufferDS) m_pBackBufferDS->Release();
    if (m_pBackBufferDSV) m_pBackBufferDSV->Release();
	if (m_pSwapChain) m_pSwapChain->Release();
	if (m_pImmediateContext) m_pImmediateContext->Release();
	if (m_pd3dDevice) m_pd3dDevice->Release(); 
    if (m_pRSCullMode[RS_CULL_NONE]) m_pRSCullMode[RS_CULL_NONE]->Release();
    if (m_pRSCullMode[RS_CULL_CCW])  m_pRSCullMode[RS_CULL_CCW]->Release();
    if (m_pRSCullMode[RS_CULL_CW])   m_pRSCullMode[RS_CULL_CW]->Release();

    if( m_pCBPerObjectBuffer ) m_pCBPerObjectBuffer->Release();
}

//--------------------------------------------------------------------------------------
// Render a frame
//--------------------------------------------------------------------------------------
void MyD3Ddevice::Render()
{
	// Clear the back buffer 
	float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red,green,blue,alpha
	m_pImmediateContext->ClearRenderTargetView(m_pBackBufferRTV, ClearColor);
    m_pImmediateContext->ClearDepthStencilView(m_pBackBufferDSV, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
    m_pImmediateContext->OMSetRenderTargets(1, &m_pBackBufferRTV, m_pBackBufferDSV);
    
    // Set the default VP
    m_pImmediateContext->RSSetViewports(1, &vp);
    // Set the default RS state
    m_pImmediateContext->RSSetState(m_pRSCullMode[RS_CULL_CCW]);

    // Set default shaders
	m_pImmediateContext->VSSetShader(m_pDefaultVS, NULL, 0);
	m_pImmediateContext->PSSetShader(m_pDefaultPS, NULL, 0);

    RenderMesh();

	// Present the information rendered to the back buffer to the front buffer (the screen)
	m_pSwapChain->Present(0, 0);
}

//--------------------------------------------------------------------------------------
// Render Mesh in mesh vector
//--------------------------------------------------------------------------------------
void MyD3Ddevice::RenderMesh()
{
    // Set default input layout
	m_pImmediateContext->IASetInputLayout(m_pDefaultLayout);
	// Set default primitive topology
	m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
	for(uint32 i=0; i<ObjArray.size(); i++)
    {
        //Set index and vertex buffer
        uint32 stride = sizeof( Vertex );
	    uint32 offset = 0;
		m_pImmediateContext->IASetIndexBuffer( ObjArray[i].indexBuff, DXGI_FORMAT_R32_UINT, 0);
		m_pImmediateContext->IASetVertexBuffers( 0, 1, &ObjArray[i].vertBuff, &stride, &offset );

        for(int iSub = 0; iSub < ; ++iSub)
	    {
		    uint32 idx = ObjArray[i].subsetMaterialArray[iSub];

		    //Set the WVP matrix and send it to the constant buffer in effect file
		    //WVP = meshWorld * camView * camProjection;
		    //cbPerObj.WVP = XMMatrixTranspose(WVP);	
		    //cbPerObj.World = XMMatrixTranspose(meshWorld);	
		    //cbPerObj.difColor = material[meshSubsetTexture[i]].difColor;
		    m_cbPerObj.hasTexture = ObjArray[i].material[idx].hasTexture;
		    m_pImmediateContext->UpdateSubresource( m_pCBPerObjectBuffer, 0, NULL, &m_cbPerObj, 0, 0 );
		    //m_pImmediateContext->VSSetConstantBuffers( 0, 1, &cbPerObjectBuffer );
		    m_pImmediateContext->PSSetConstantBuffers( 0, 1, &m_pCBPerObjectBuffer );
            
		    if(ObjArray[i].material[idx].hasTexture)
			    m_pImmediateContext->PSSetShaderResources( 0, 1, &ObjArray[i].meshSRV[ObjArray[i].material[idx].texArrayIndex] );

		    int indexStart = ObjArray[i].subsetIndexStart[iSub];
		    int indexDrawAmount =  ObjArray[i].subsetIndexStart[iSub+1] - ObjArray[i].subsetIndexStart[iSub];
			m_pImmediateContext->DrawIndexed( indexDrawAmount, indexStart, 0 );
	    }
    }
}