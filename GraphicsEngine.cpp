#include "GraphicsEngine.h"


GraphicsEngine::GraphicsEngine()
{
	// Set D3Dpointer to null
	m_d3d = 0;
	// Initialize camera, model, shader
	m_Camera = 0;
	m_Model = 0;
	m_LightShader = 0;
	m_Light = 0;
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
	char modelFilename[128];
	
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
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

	// Set the file name of the model.
	strcpy_s(modelFilename, "Models/cube.txt");
	
	// Create and initilize model object
	m_Model = new Model;

	// Set the name of the texture file that we will be loading.
	strcpy_s(textureFilename, "Textures/stone01.tga");

	res = m_Model->Initialize(m_d3d->GetDevice(), m_d3d->GetDeviceContext(), modelFilename, textureFilename);	if (!res)
	{
		MessageBox(hwnd, L"Could not initialize the model object", L"Error", MB_OK);
		return false;
	}

	// Create and initialize the light shader object.
	m_LightShader = new LightShader;

	res = m_LightShader->Initialize(m_d3d->GetDevice(), hwnd);
	if (!res)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	// Create and initialize the light object.
	m_Light = new Light;

	m_Light->SetDiffuseColor(1.0f, 0.0f, 0.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);

	return true;
}


void GraphicsEngine::Shutdown()
{
	// Release the light object.
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// Release the light shader object.
	if (m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
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
	static float rotation = 0.0f;

	// Update the rotation variable each frame. (Spins the model)
	rotation -= 0.1174532925f * 0.1f;
	if (rotation < 0.0f)
	{
		rotation += 360.0f;
	}

	// Render the graphics
	result = Render(rotation);
	if (!result)
	{
		return false;
	}
	return true;
}


bool GraphicsEngine::Render(float rotation)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, rotateMatrix, translateMatrix, scaleMatrix, srMatrix; worldMatrix, viewMatrix, projectionMatrix;
	bool res;

	// Clear buffer to begin scene
	m_d3d->BeginFrame(0.0f, 0.0f, 0.0f, 1.f);

	// Generate view matrix based on camera position
	m_Camera->Render();
	// Get matrices from camera and d3d
	m_d3d->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_d3d->GetProjectionMatrix(projectionMatrix);

	// Transform 1 - Rotation, translate to left
	rotateMatrix = XMMatrixRotationY(rotation);  // Build the rotation matrix.
	translateMatrix = XMMatrixTranslation(-2.0f, 0.0f, 0.0f);  // Build the translation matrix.

	// Multiply them together to create the final world transformation matrix.
	worldMatrix = XMMatrixMultiply(rotateMatrix, translateMatrix);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_Model->Render(m_d3d->GetDeviceContext());

	// Render the model using the light shader.
	res = m_LightShader->Render(m_d3d->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(),
		m_Light->GetDirection(), m_Light->GetDiffuseColor());
	if (!res)
	{
		return false;
	}

	// Transform 2 - Rotate, translate to right, scale down
	scaleMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);  // Build the scaling matrix.
	rotateMatrix = XMMatrixRotationY(rotation);  // Build the rotation matrix.
	translateMatrix = XMMatrixTranslation(2.0f, 0.0f, 0.0f);  // Build the translation matrix.

	// Multiply the scale, rotation, and translation matrices together to create the final world transformation matrix.
	srMatrix = XMMatrixMultiply(scaleMatrix, rotateMatrix);
	worldMatrix = XMMatrixMultiply(srMatrix, translateMatrix);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_Model->Render(m_d3d->GetDeviceContext());

	// Render the model using the light shader.
	res = m_LightShader->Render(m_d3d->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(),
		m_Light->GetDirection(), m_Light->GetDiffuseColor());
	if (!res)
	{
		return false;
	}

	// Present to the screen
	m_d3d->EndFrame();

	return true;
}