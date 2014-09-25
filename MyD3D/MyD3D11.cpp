//--------------------------------------------------------------------------------------
// File: MyD3D11.cpp
//
// This application displays a triangle using Direct3D 11
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include <windows.h>
#include "resource.h"
#include "MyD3Ddevice.h"
#include "MyObjLoader.h"


//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
HINSTANCE               g_hInst = NULL;
HWND                    g_hWnd = NULL;

OPENFILENAME ofn;


//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow );
LRESULT CALLBACK    WndProc( HWND, UINT, WPARAM, LPARAM );
BOOL    CALLBACK    AboutDlgProc(HWND, UINT, WPARAM, LPARAM);

void PopFileInitialize(HWND, char*);


//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	UNREFERENCED_PARAMETER( hPrevInstance );
    UNREFERENCED_PARAMETER( lpCmdLine );

	MyD3Ddevice MyD3DRenderer;

    if( FAILED( InitWindow( hInstance, nCmdShow ) ) )
        return 0;

	if (FAILED(MyD3DRenderer.InitDevice(g_hWnd)))
    {
		MyD3DRenderer.CleanupDevice();
        return 0;
    }

    // Main message loop
    MSG msg = {0};
    while( WM_QUIT != msg.message )
    {
        if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
			MyD3DRenderer.Render();
        }
    }

	MyD3DRenderer.CleanupDevice();

    return ( int )msg.wParam;
}


//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow )
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof( WNDCLASSEX );
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_WALLE);
    wcex.hCursor = LoadCursor( NULL, IDC_ARROW );
    wcex.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
	wcex.lpszMenuName = MAKEINTRESOURCE(IDM_MAIN);
    wcex.lpszClassName = L"MyD3DWindowClass";
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_WALLE);
    if( !RegisterClassEx( &wcex ) )
        return E_FAIL;

    // Create window
    g_hInst = hInstance;
    RECT rc = { 0, 0, 1024, 768 };
    AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
    g_hWnd = CreateWindow( L"MyD3DWindowClass", L"Global illumination",
                           WS_OVERLAPPEDWINDOW,
						   CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, LoadMenu(hInstance, MAKEINTRESOURCE(IDM_MAIN)), hInstance,
                           NULL );
    if( !g_hWnd )
        return E_FAIL;

    ShowWindow( g_hWnd, nCmdShow );

    return S_OK;
}

//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT ps;
    HDC hdc;

	static HINSTANCE hInstance;
    static char szFilename[MAX_PATH] = {'\0'};

    switch( message )
    {
		case WM_CREATE:
			hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
			PopFileInitialize(hWnd,szFilename);
			break;
		case WM_PAINT:
            hdc = BeginPaint( hWnd, &ps );
            EndPaint( hWnd, &ps );
            break;
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDM_OPEN:
				if (GetOpenFileName(&ofn))
				{
				}
				break;
			case IDM_EXIT:
				DestroyWindow(hWnd);
				break;
			case IDM_ABOUT:
				DialogBox(hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, AboutDlgProc);
				break;
			default:
				break;
			}
			break;
		case WM_CHAR:
			switch (static_cast<int>(wParam))
			{
			case VK_ESCAPE:
				PostMessage(hWnd, WM_CLOSE, 0, 0);
				break;
			default :
				break;
			}
			break;
        case WM_DESTROY:
            PostQuitMessage( 0 );
            break;

        default:
            return DefWindowProc( hWnd, message, wParam, lParam );
    }

    return 0;
}

BOOL CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
			return TRUE;
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case   IDOK:
			case   IDCANCEL:
				EndDialog(hDlg, 0);
				return TRUE;
			}
			break;
	}
	return FALSE;
}

void PopFileInitialize(HWND hwnd, char* szFilename)
{

	static TCHAR szFilter[] = L"OBJ Files (*.OBJ)\0*.obj\0"  \
							  L"X Files (*.X)\0*.x\0" \
							  L"All Files (*.*)\0*.*\0\0";

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.hInstance = NULL;
	ofn.lpstrFilter = szFilter;
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter = 0;
	ofn.nFilterIndex = 0;
	ofn.lpstrFile = szFilename;             
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFileTitle = NULL;                 
	ofn.nMaxFileTitle = MAX_PATH;
	ofn.lpstrInitialDir = L".\\Media";
	ofn.lpstrTitle = L"Open Model File";
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER;
	ofn.nFileOffset = 0;
	ofn.nFileExtension = 0;
	ofn.lpstrDefExt = L"obj";
	ofn.lCustData = 0L;
	ofn.lpfnHook = NULL;
	ofn.lpTemplateName = NULL;
}


