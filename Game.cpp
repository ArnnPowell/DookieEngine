#include "Game.h"

Game::Game()
{
	// Initialize pointers to null 
	m_Input = 0;
	m_GraphicsEngine = 0;
}

Game::Game(const Game& other) {};
Game::~Game() {};

bool Game::Initialize()
{
	int screenWidth, screenHeight;
	bool res;

	// Initialize screen dimensions to 0
	screenWidth = 0;
	screenHeight = 0;

	// Initialize window API
	InitializeWindows(screenWidth, screenHeight);

	// Create and Initialize Input object
	m_Input = new Input;
	m_Input->Initialize();

	// Create and Initialize GraphicsEngine object
	m_GraphicsEngine = new GraphicsEngine;
	res = m_GraphicsEngine->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!res)
	{
		return false;
	}

	return true;
}

void Game::Shutdown()
{
	// Release GraphicEngine
	if (m_GraphicsEngine)
	{
		m_GraphicsEngine->Shutdown();
		delete m_GraphicsEngine;
		m_GraphicsEngine = 0;
	}

	// Release PlayerControls
	if (m_Input)
	{
		delete m_Input;
		m_Input = 0;
	}

	// Shutdown the game
	ShutdownWindows();

	return;
}

void Game::Run()
{
	MSG msg;
	bool done, result;

	// Initialize message structure
	ZeroMemory(&msg, sizeof(MSG));

	// Loop until quit message
	done = false;
	while (!done)
	{
		// Handle windows messages
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows wants to end
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// Run Game Loop
			result = Loop();
			if (!result)
			{
				done = true;
			}
		}
	}

	return;
}

bool Game::Loop()
{
	bool result;

	// Check for exit key press
	if (m_Input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	// GraphicEngine loop
	result = m_GraphicsEngine->Frame();
	if (!result)
	{
		return false;
	}

	return true;
}

LRESULT CALLBACK Game::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		// Check for key press
		case WM_KEYDOWN:
		{
			// If key pressed send it to Input
			m_Input->KeyDown((unsigned int)wparam);
			return 0;
		}
		// Check for key release
		case WM_KEYUP:
		{
			// If key released send it to Input
			m_Input->KeyUp((unsigned int)wparam);
			return 0;
		}
		// Any other message send to message handler
		default:
		{
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}
}

void Game::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX win;
	DEVMODE dmScreenSettings;
	int posX, posY;

	// Get pointer to this object
	ApplicationHandle = this;
	// Get the instance of this Game
	m_hinstance = GetModuleHandle(NULL);
	// Give Game Name
	m_GraphicsEngineName = L"DookieEngine";

	// Setup the window class - default settings
	win.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	win.lpfnWndProc = WndProc;
	win.cbClsExtra = 0;
	win.cbWndExtra = 0;
	win.hInstance = m_hinstance;
	win.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	win.hIconSm = win.hIcon;
	win.hCursor = LoadCursor(NULL, IDC_ARROW);
	win.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	win.lpszMenuName = NULL;
	win.lpszClassName = m_GraphicsEngineName;
	win.cbSize = sizeof(WNDCLASSEX);

	// Register the window class
	RegisterClassEx(&win);

	// Determine the resolution of the clients desktop screen
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Setup screen depending on Fullscreen or windowed
	if (FULL_SCREEN)
	{
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
	}
	else
	{
		// If windowed then set it to 800x600 resolution.
		screenWidth = 800;
		screenHeight = 600;

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}
	
	// Create the window with the screen settings and get the handle to it.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_GraphicsEngineName, m_GraphicsEngineName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// Open window  and set it as main focus.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// Hide the mouse cursor.
	ShowCursor(false);

	return;
}

void Game::ShutdownWindows()
{
	// Show mouse cursor
	ShowCursor(true);

	// Fix display settings if leaving full screen
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// Remove the window.
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// Remove the application instance.
	UnregisterClass(m_GraphicsEngineName, m_hinstance);
	m_hinstance = NULL;

	// Release the pointer to this class.
	ApplicationHandle = NULL;

	return;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		// Check if the window is being destroyed.
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		// Check if the window is being closed.
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		// All other messages pass to the message handler in the system class.
		default:
		{
			return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}
