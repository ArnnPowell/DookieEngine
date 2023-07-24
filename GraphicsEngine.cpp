#include "GraphicsEngine.h"


GraphicsEngine::GraphicsEngine()
{
	// Set D3Dpointer to null
	m_d3d = 0;
}


GraphicsEngine::GraphicsEngine(const GraphicsEngine& other)
{
}


GraphicsEngine::~GraphicsEngine()
{
}


bool GraphicsEngine::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;
	
	// Initialize Direct3D object
	m_d3d = new D3D;

	result = m_d3d->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}
	return true;
}


void GraphicsEngine::Shutdown()
{
	// Release D3D object
	if (m_d3d)
	{
		m_d3d->Shutdown();
		delete m_d3d;
		m_d3d = 0;
	}
	return;
}


bool GraphicsEngine::Frame()
{
	bool result;

	// Render the graphics
	result = Render();
	if (!result)
	{
		return false;
	}
	return true;
}


bool GraphicsEngine::Render()
{
	// Clear buffer to begin scene
	m_d3d->BeginFrame(0.5f, 0.5f, 0.5f, 1.f);

	// Present to the screen
	m_d3d->EndFrame();

	return true;
}