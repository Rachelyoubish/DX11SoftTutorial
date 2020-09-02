#pragma comment(lib, "d3d11.lib")

#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

// Global declarations.
IDXGISwapChain* SwapChain;
ID3D11Device* d3d11Device;
ID3D11DeviceContext* d3d11DeviceContext;
ID3D11RenderTargetView* renderTargetView;

float red = 0.0f;
float green = 0.0f;
float blue = 0.0f;
int colormodr = 1;
int colormodg = 1;
int colormodb = 1;

LPCTSTR WndClassName = "FirstWindow";	// Define our window class name.
HWND hwnd = NULL;						// Sets our window handle to null.

const int Width = 800;	// Window width.
const int Height = 600;	// Window height.

// Function prototypes.
bool InitializeDirect3d11App( HINSTANCE hInstance );
void ReleaseObjects();
bool InitScene();
void UpdateScene();
void DrawScene();

bool InitializeWindow( HINSTANCE hInstance,
	int ShowWnd,
	int width, int height,
	bool windowed );

int MessageLoop();

LRESULT CALLBACK WndProc( HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam );

// Entry point.
int WINAPI WinMain( HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nShowCmd )
{
	// Initialize Window.
	if ( !InitializeWindow( hInstance, nShowCmd, Width, Height, true ) )
	{
		MessageBox( 0, "Window Initialization - Failed",
			"Error", MB_OK );
		return 0;
	}

	// Initialize Direct3D.
	if ( !InitializeDirect3d11App( hInstance ) )
	{
		MessageBox( 0, "Scene Initializtion - Failed",
			"Error", MB_OK );
		return 0;
	}

	// Main loop.
	MessageLoop();

	ReleaseObjects();

	return 0;
}

bool InitializeWindow( HINSTANCE hInstance,
	int ShowWnd,
	int width, int height,
	bool windowed )
{
	// Setting up window.

	WNDCLASSEX wc = {};

	wc.cbSize = sizeof( WNDCLASSEX );
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = NULL;
	wc.cbWndExtra = NULL;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );
	wc.hCursor = LoadCursor( NULL, IDC_ARROW );
	wc.hbrBackground = (HBRUSH)( COLOR_WINDOW + 2 );
	wc.lpszMenuName = NULL;
	wc.lpszClassName = WndClassName;
	wc.hIconSm = LoadIcon( NULL, IDI_APPLICATION );

	// Register our window class.
	if ( !RegisterClassEx( &wc ) )
	{
		MessageBox( NULL, "Error registering class",
			"Error", MB_OK | MB_ICONERROR );
		return false;
	}

	// Creating the window. 
	hwnd = CreateWindowEx( 0, WndClassName,
						   "Window Title",
						   WS_OVERLAPPEDWINDOW,
						   CW_USEDEFAULT, CW_USEDEFAULT, width, height,
						   NULL, NULL, hInstance, NULL );

	// Check if the window has been created.
	if ( !hwnd )
	{
		MessageBox( NULL, "Error creating window",
					"Error", MB_OK | MB_ICONERROR );
		return false;
	}

	ShowWindow( hwnd, ShowWnd );
	UpdateWindow( hwnd );

	// If no errors occured, return true.
	return true;
}

bool InitializeDirect3d11App( HINSTANCE hInstance )
{
	HRESULT hr;

	// Describe our Buffer.
	DXGI_MODE_DESC bufferDesc;

	ZeroMemory( &bufferDesc, sizeof( DXGI_MODE_DESC ) );

	bufferDesc.Width = Width;
	bufferDesc.Height = Height;
	bufferDesc.RefreshRate.Numerator = 60;
	bufferDesc.RefreshRate.Denominator = 1;
	bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Describe our SwapChain.
	DXGI_SWAP_CHAIN_DESC swapChainDesc;

	ZeroMemory( &swapChainDesc, sizeof( DXGI_SWAP_CHAIN_DESC ) );

	swapChainDesc.BufferDesc = bufferDesc;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Create our SwapChain.
	hr = D3D11CreateDeviceAndSwapChain( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL,
		D3D11_SDK_VERSION, &swapChainDesc, &SwapChain, &d3d11Device, NULL, &d3d11DeviceContext );

	// Create our BackBuffer.
	ID3D11Texture2D* BackBuffer;
	hr = SwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (void**)&BackBuffer );

	// Create our Render Target.
	hr = d3d11Device->CreateRenderTargetView( BackBuffer, NULL, &renderTargetView );
	BackBuffer->Release();

	// Set our Render Target.
	d3d11DeviceContext->OMSetRenderTargets( 1, &renderTargetView, NULL );

	return true;
}

void ReleaseObjects()
{
	// Release the COM Objects we created.
	SwapChain->Release();
	d3d11Device->Release();
	d3d11DeviceContext->Release();
}

bool InitScene()
{
	return true;
}

void UpdateScene()
{
	//Update the colors of our scene
	red += colormodr * 0.00005f;
	green += colormodg * 0.00002f;
	blue += colormodb * 0.00001f;

	if ( red >= 1.0f || red <= 0.0f )
		colormodr *= -1;
	if ( green >= 1.0f || green <= 0.0f )
		colormodg *= -1;
	if ( blue >= 1.0f || blue <= 0.0f )
		colormodb *= -1;
}

void DrawScene()
{
	// Clear our backbuffer to the updated color.
	XMFLOAT4 bgColor( red, green, blue, 1.0f );

	d3d11DeviceContext->ClearRenderTargetView( renderTargetView, (FLOAT*)&bgColor );

	// Present the backbuffer to the screen.
	SwapChain->Present( 0, 0 );
}

// The main message loop.
int MessageLoop()
{
	MSG msg;							// Create a new message structure.
	ZeroMemory( &msg, sizeof( MSG ) );	// Clear message structure to NULL.
	
	while ( true )
	{
		// If there was a window message. . .
		if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			if ( msg.message == WM_QUIT )	// If the message was WM_QUIT, exit the loop.
				break;
			TranslateMessage( &msg );		// Translate the loop.
			DispatchMessage( &msg );		// Send the message to default windows procedure.
		}
		else
		{
			UpdateScene();
			DrawScene();
		}
	}
	// Return the message.
	return (int)msg.wParam;
}

// Default windows procedure.
LRESULT CALLBACK WndProc( HWND hwnd,
			UINT msg,
			WPARAM wParam,
			LPARAM lParam )
{
	// Check message.
	switch ( msg )
	{
		case WM_KEYDOWN:
		{
			if ( wParam == VK_ESCAPE ) {
				DestroyWindow( hwnd );
			}
			return 0;
		}
		case WM_DESTROY:
		{
			PostQuitMessage( 0 );
			return 0;
		}
	}
	// Return the message for windows to handle it.
	return DefWindowProc( hwnd,
		     msg,
		     wParam,
		     lParam );
}