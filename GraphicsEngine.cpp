#include "GraphicsEngine.h"


GraphicsEngine::GraphicsEngine()
{
	// Set D3Dpointer to null
	m_d3d = 0;
	// Initialize camera, model, color shader
	m_Camera = 0;
	m_Model = 0;
	m_ColorShader = 0;
}


GraphicsEngine::GraphicsEngine(const GraphicsEngine& other)
{
}


GraphicsEngine::~GraphicsEngine()
{
}


bool GraphicsEngine::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool res;
	
	// Initialize Direct3D object
	m_d3d = new D3D;

	res = m_d3d->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!res)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	// Create camera
	m_Camera = new Camera;
	// Set default position of camera
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);
	
	// Create and initilize model object
	m_Model = new Model;
	res = m_Model->Initialize(m_d3d->GetDevice());
	if (!res)
	{
		MessageBox(hwnd, L"Could not initialize the model object", L"Error", MB_OK);
		return false;
	}

	// Create and initialize the color shader object
	m_ColorShader = new ColorShader;
	res = m_ColorShader->Initialize(m_d3d->GetDevice(), hwnd);
	if (!res)
	{
		MessageBox(hwnd, L"Could not initialize the color shader object", L"Error", MB_OK);
		return false;
	}

	return true;
}


void GraphicsEngine::Shutdown()
{
	// Release the color shader
	if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
	}

	// Release the model
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	// Release the camera
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}
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
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool res;

	// Clear buffer to begin scene
	m_d3d->BeginFrame(0.0f, 0.0f, 0.0f, 1.f);

	// Generate view matrix based on camera position
	m_Camera->Render();
	// Get matrices from camera and d3d
	m_d3d->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_d3d->GetProjectionMatrix(projectionMatrix);

	// Put model buffers on pipline
	m_Model->Render(m_d3d->GetDeviceContext());

	// Render the model using ColorShader
	res = m_ColorShader->Render(m_d3d->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	if (!res)
	{
		return false;
	}

	// Present to the screen
	m_d3d->EndFrame();

	return true;
}