#include <Windows.h>

// Helper variables/constants.
LPCTSTR WndClassName = "FirstWindow";	// Define our window class name.
HWND hwnd = NULL;						// Sets our window handle to null.

const int Width = 800;	// Window width.
const int Height = 600;	// Window height.

// Helper Functions.
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

	// Main loop.
	MessageLoop();

	return 0;
}

bool InitializeWindow( HINSTANCE hInstance,
	int ShowWnd,
	int width, int height,
	bool windowed )
{
	// Setting up window.

	WNDCLASSEX wc = {};

	wc.cbSize = sizeof( wc );
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
			// Run game code.
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
			if ( wParam == VK_ESCAPE )
			{
				if ( MessageBox( 0, "Are you sure you want to exit?",
					"Really?", MB_YESNO | MB_ICONQUESTION ) == IDYES )
					
					// Release the window's allocated memory.
					DestroyWindow( hwnd );
				break;
			}
		}
		case WM_DESTROY:
		{
			PostQuitMessage( 0 );
		}
	}
	// Return the message for windows to handle it.
	return DefWindowProc( hwnd,
		     msg,
		     wParam,
		     lParam );
}