#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

using namespace DirectX;

// Global declarations.
IDXGISwapChain* SwapChain;
ID3D11Device* d3d11Device;
ID3D11DeviceContext* d3d11DeviceContext;
ID3D11RenderTargetView* renderTargetView;

ID3D11Buffer* triangleVertBuffer;
ID3D11VertexShader* VS;
ID3D11PixelShader* PS;
ID3DBlob* VS_Buffer;
ID3DBlob* PS_Buffer;
ID3D11InputLayout* vertLayout;

LPCTSTR WndClassName = "FirstWindow";	// Define our window class name.
HWND hwnd = nullptr;					// Sets our window handle to nullptr.
HRESULT hr;

const int Width = 800;	// Window width.
const int Height = 600;	// Window height.

// Function prototypes.
bool InitializeDirect3d11App( HINSTANCE hInstance );
void CleanUp();
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

// Vertex Structure and Vertex Layout (Input Layout)
struct Vertex	// Overloaded Vertex Structure
{
	Vertex() {}
	Vertex( float x, float y, float z )
		: pos( x, y, z ) {}

	XMFLOAT3 pos;
};

D3D11_INPUT_ELEMENT_DESC layout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
};
UINT numElements = ARRAYSIZE( layout );

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

	// Initialize Scene.
	if ( !InitScene() )
	{
		MessageBox( 0, "Scene Initialization - Failed",
			"Error", MB_OK );
		return 0;
	}

	// Main loop.
	MessageLoop();

	CleanUp();

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
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon( nullptr, IDI_APPLICATION );
	wc.hCursor = LoadCursor( nullptr, IDC_ARROW );
	wc.hbrBackground = (HBRUSH)( COLOR_WINDOW + 2 );
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = WndClassName;
	wc.hIconSm = LoadIcon( nullptr, IDI_APPLICATION );

	// Register our window class.
	if ( !RegisterClassEx( &wc ) )
	{
		MessageBox( nullptr, "Error registering class",
			"Error", MB_OK | MB_ICONERROR );
		return false;
	}

	// Creating the window. 
	hwnd = CreateWindowEx( 0, WndClassName,
		"A Very Interesting Title",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, width, height,
		nullptr, nullptr, hInstance, nullptr );

	// Check if the window has been created.
	if ( !hwnd )
	{
		MessageBox( nullptr, "Error creating window",
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
	hr = D3D11CreateDeviceAndSwapChain( nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0,
		D3D11_SDK_VERSION, &swapChainDesc, &SwapChain, &d3d11Device, nullptr, &d3d11DeviceContext );

	// Create our BackBuffer.
	ID3D11Texture2D* BackBuffer;
	hr = SwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (void**)&BackBuffer );

	// Create our Render Target.
	hr = d3d11Device->CreateRenderTargetView( BackBuffer, nullptr, &renderTargetView );
	BackBuffer->Release();

	// Set our Render Target.
	d3d11DeviceContext->OMSetRenderTargets( 1, &renderTargetView, nullptr );

	return true;
}

void CleanUp()
{
	// Release the COM Objects we created.
	SwapChain->Release();
	d3d11Device->Release();
	d3d11DeviceContext->Release();
	renderTargetView->Release();

	triangleVertBuffer->Release();
	VS->Release();
	PS->Release();
	VS_Buffer->Release();
	PS_Buffer->Release();
	vertLayout->Release();
}

bool InitScene()
{
	// Compile Shaders from shader file.
	hr = D3DCompileFromFile( L"Effects.fx", 0, 0, "VS", "vs_4_0", 0, 0, &VS_Buffer, nullptr );
	hr = D3DCompileFromFile( L"Effects.fx", 0, 0, "PS", "ps_4_0", 0, 0, &PS_Buffer, nullptr );

	// Create the Shader Objects.
	hr = d3d11Device->CreateVertexShader( VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), nullptr, &VS );
	hr = d3d11Device->CreatePixelShader( PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(), nullptr, &PS );

	// Set Vertex and Pixel Shaders.
	d3d11DeviceContext->VSSetShader( VS, 0, 0 );
	d3d11DeviceContext->PSSetShader( PS, 0, 0 );

	// Create the Vertex buffer.
	Vertex v[] =
	{
		Vertex( 0.0f, 0.5f, 0.5f ),
		Vertex( 0.5f, -0.5f, 0.5f ),
		Vertex( -0.5f, -0.5f, 0.5f ),
	};

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory( &vertexBufferDesc, sizeof( vertexBufferDesc ) );

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof( Vertex ) * 3;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;

	ZeroMemory( &vertexBufferData, sizeof( vertexBufferData ) );
	vertexBufferData.pSysMem = v;
	hr = d3d11Device->CreateBuffer( &vertexBufferDesc, &vertexBufferData, &triangleVertBuffer );

	// Set the Vertex buffer.
	UINT stride = sizeof( Vertex );
	UINT offset = 0;
	d3d11DeviceContext->IASetVertexBuffers( 0, 1, &triangleVertBuffer, &stride, &offset );

	// Create the Input Layout.
	d3d11Device->CreateInputLayout( layout, numElements, VS_Buffer->GetBufferPointer(),
		VS_Buffer->GetBufferSize(), &vertLayout );

	// Set the Input Layout.
	d3d11DeviceContext->IASetInputLayout( vertLayout );

	// Set Primitive Topology.
	d3d11DeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	// Create the Viewport.
	D3D11_VIEWPORT viewport;
	ZeroMemory( &viewport, sizeof( D3D11_VIEWPORT ) );

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = Width;
	viewport.Height = Height;

	// Set the Viewport.
	d3d11DeviceContext->RSSetViewports( 1, &viewport );

	return true;
}

void UpdateScene()
{

}

void DrawScene()
{
	// Clear our backbuffer to the updated color.
	float bgColor[4] = { ( 0.0f, 0.0f, 0.0f, 0.0f ) };
	d3d11DeviceContext->ClearRenderTargetView( renderTargetView, bgColor );

	// Draw the triangle.
	d3d11DeviceContext->Draw( 3, 0 );

	// Present the backbuffer to the screen.
	SwapChain->Present( 0, 0 );
}

// The main message loop.
int MessageLoop()
{
	MSG msg;							// Create a new message structure.
	ZeroMemory( &msg, sizeof( MSG ) );	// Clear message structure to nullptr.

	while ( true )
	{
		// If there was a window message. . .
		if ( PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE ) )
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