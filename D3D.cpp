#include"D3D.h"

D3D::D3D()
{
	// Set pointers to null
	m_swapChain = 0;
	m_device = 0;
	m_deviceContext = 0;
	m_renderTargetView = 0;
	m_depthStencilBuffer = 0;
	m_depthStencilState = 0;
	m_depthStencilView = 0;
	m_rasterState = 0;
}

D3D::D3D(const D3D& other) {};

D3D::~D3D() {};

bool D3D::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear)
{
	HRESULT res;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, i, numerator, denominator;
	unsigned long long stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	float fov, aspectRatio;

	// Store vsync setting
	m_vsync_enabled = vsync;

	// Graphic Card Information
	// Direct3D graphic interface factory
	res = CreateDXGIFactory(_uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(res))
	{
		return false;
	}

	// Create adaptor for graphic interface / video card
	res = factory->EnumAdapters(0, &adapter);
	if (FAILED(res))
	{
		return false;
	}

	// Enumerate primary adapter output / monitor
	res = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(res))
	{
		return false;
	}

	// Get number of modes that fit for monitor
	res = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(res))
	{
		return false;
	}

	// Create a list to hold all the possible display modes for this monitor/video card combination.
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	// Now fill the display mode list structures.
	res = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(res))
	{
		return false;
	}

	// Store the numerator and denominator of the refresh rate for the monitor.
	for (i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if (displayModeList[i].Height == (unsigned int)screenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	// Get the adapter (video card) description.
	res = adapter->GetDesc(&adapterDesc);
	if (FAILED(res))
	{
		return false;
	}

	// Store video card memory
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// Store name of video card
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		return false;
	}

	// Release factory, adaptor, mode list
	delete[] displayModeList;
	displayModeList = 0;
	adapterOutput->Release();
	adapterOutput = 0;
	adapter->Release();
	adapter = 0;
	factory->Release();
	factory = 0;

	////////////////////////////////
	// INITIALIZATION OF DIRECT3D //
	////////////////////////////////

	////////////////////////////////
	// SwapChain
	// Initialize description
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	// Set to a single back buffer
	swapChainDesc.BufferCount = 1;
	// Set the width and height of the back buffer
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;
	// Set regular 32-bit surface for the back buffer
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// Set back buffer refresh rate
	if (m_vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}
	// Set back buffer usage
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	// Set window handle to render to
	swapChainDesc.OutputWindow = hwnd;
	// Set multisampling - off
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	// Set fullscreen or windowed
	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}
	// Set the scan line ordering and scaling to unspecified
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	// Discard the back buffer contents after presenting
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	// Don't set the advanced flags
	swapChainDesc.Flags = 0;

	// Set feature level to DirectX11
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	// Create Device, Device Context, SwapChain
	res = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
	if (FAILED(res))
	{
		return false;
	}

	// Get pointer to the back buffer
	res = m_swapChain->GetBuffer(0, _uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(res))
	{
		return false;
	}
	// Create the render target view with back buffer pointer
	res = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if (FAILED(res))
	{
		return false;
	}
	// Release pointer to back buffer
	backBufferPtr->Release();
	backBufferPtr = 0;

	////////////////////////////////
	// DEPTH BUFFER
	// Initialize description
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	// Set up the description of the depth buffer.
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;
	// Create texture for depthbuffer
	res = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if (FAILED(res))
	{
		return false;
	}

	////////////////////////////////
	// DEPTH STENCIL
	// Initialize description
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	// Create depth stencil state
	res = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if (FAILED(res))
	{
		return false;
	}

	// Use device context to set the depth stencil stat
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	// Initialize depth stencil view
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	// Set up the depth stencil view description
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	// Create the depth stencil view
	res = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(res))
	{
		return false;
	}

	// Bind the render target view and depth stencil buffer to the output render pipeline
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	////////////////////////////////
	// Rasterizer State
	// Initialize description
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;
	// Create the rasterizer state
	res = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if (FAILED(res))
	{
		return false;
	}

	// Now set the rasterizer state
	m_deviceContext->RSSetState(m_rasterState);

	////////////////////////////////
	// VIEWPORT
	// Setup the viewport for rendering
	m_viewport.Width = (float)screenWidth;
	m_viewport.Height = (float)screenHeight;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;

	// Create viewport
	m_deviceContext->RSSetViewports(1, &m_viewport);

	// Projection Matrix Setup
	fov = 3.141592654f / 4.0f;
	aspectRatio = (float)screenWidth / (float)screenHeight;

	// Create the projection matrix
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fov, aspectRatio, screenNear, screenDepth);

	// Initialize world matrix
	m_worldMatrix = XMMatrixIdentity();

	// Create an orthographic projection matrix - 2D Rendering(UI)
	m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	return true;
}

void D3D::Shutdown()
{
	// Set to windowed mode to avoid exceptions
	if (m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, NULL);
	}

	// Release all pointers
	if (m_rasterState)
	{
		m_rasterState->Release();
		m_rasterState = 0;
	}
	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}
	if (m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = 0;
	}
	if (m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}
	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}
	if (m_deviceContext)
	{
		m_deviceContext->Release();
		m_deviceContext = 0;
	}
	if (m_device)
	{
		m_device->Release();
		m_device = 0;
	}
	if (m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = 0;
	}

	return;
}

// BeginFrame - start of each frame to draw - buffers blank and ready to draw
void D3D::BeginFrame(float red, float green, float blue, float alpha)
{
	float color[4];

	// Set clear color for buffer
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;
	// Clear back buffer
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);
	// Clear the depth buffer
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.f, 0);

	return;
}

// EndFrame - tells swapchain to present when drawing complete
void D3D::EndFrame()
{
	// Present the back buffer to screen
	if (m_vsync_enabled)
	{
		// Lock to screen refresh rate
		m_swapChain->Present(1, 0);
	}
	else
	{
		// Present as fast as possible
		m_swapChain->Present(0, 0);
	}

	return;
}

// Helper Functions
ID3D11Device* D3D::GetDevice()
{
	return m_device;
}


ID3D11DeviceContext* D3D::GetDeviceContext()
{
	return m_deviceContext;
}

void D3D::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
	return;
}

void D3D::GetWorldMatrix(XMMATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
	return;
}


void D3D::GetOrthoMatrix(XMMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
	return;
}

// Video Card name and memory amount - debugging
void D3D::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
	return;
}

// Render to texture helpers
void D3D::SetBackBufferRenderTarget()
{
	// Bind the render target view and depth stencil buffer to the output render pipeline.
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	return;
}

void D3D::ResetViewport()
{
	// Set the viewport.
	m_deviceContext->RSSetViewports(1, &m_viewport);

	return;
}