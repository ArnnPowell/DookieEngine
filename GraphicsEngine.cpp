#include "GraphicsEngine.h"


GraphicsEngine::GraphicsEngine()
{
	// Set D3Dpointer to null
	m_d3d = 0;
	// Initialize camera, model, color shader
	m_Camera = 0;
	m_Model = 0;
	m_TextureShader = 0;
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
	char textureFilename[128];
	
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

	// Set the name of the texture file that we will be loading.
	strcpy_s(textureFilename, "Textures/stone01.tga");

	res = m_Model->Initialize(m_d3d->GetDevice(), m_d3d->GetDeviceContext(), textureFilename); // added texture filename
	if (!res)
	{
		MessageBox(hwnd, L"Could not initialize the model object", L"Error", MB_OK);
		return false;
	}

	// Create and initialize the texture shader object.
	m_TextureShader = new TextureShader;

	res = m_TextureShader->Initialize(m_d3d->GetDevice(), hwnd);
	if (!res)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}


void GraphicsEngine::Shutdown()
{
	// Release the texture shader object.
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
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

	// Render the model using the texture shader.
	res = m_TextureShader->Render(m_d3d->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture());
	if (!res)
	{
		return false;
	}

	// Present to the screen
	m_d3d->EndFrame();

	return true;
}