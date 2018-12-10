#include "d3d.h"

D3D::D3D()
{
	m_swapChain = NULL;
	m_device = NULL;
	m_deviceContext = NULL;
	m_renderTargetView = NULL;
	m_depthStencilBuffer = NULL;
	m_depthStencilState3D = NULL;
	m_depthStencilView = NULL;
	m_rasterStateBackCull = NULL;
	m_rasterStateFrontCull = NULL;
	m_depthStencilState2D = NULL;
	m_alphaEnableBlendingState = NULL;
	m_alphaDisableBlendingState = NULL;
}

D3D::D3D(const D3D &other)
{
	UNREFERENCED_PARAMETER(other);
}

D3D::~D3D()
{
}

bool D3D::Initialize(HWND hwnd, bool vsync, bool fullscreen, float screenDepth, float screenNear)
{

	m_vsyncEnabled = vsync;

	RECT rc;
	GetClientRect(hwnd, &rc);
	m_screenWidth = rc.right - rc.left;
	m_screenHeight = rc.bottom - rc.top;

	UINT creationFlags = 0;
#ifdef _DEBUG
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT result = S_OK;

	D3D_DRIVER_TYPE driverTypes[] = {
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_SOFTWARE};

	UINT totatDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0};
	UINT totalFeaturedLevels = ARRAYSIZE(featureLevels);

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = m_screenWidth;
	swapChainDesc.BufferDesc.Height = m_screenHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = vsync ? 60 : 0;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = !fullscreen;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	for (UINT driver = 0; driver < totatDriverTypes; driver++)
	{
		result = D3D11CreateDeviceAndSwapChain(NULL, driverTypes[driver], NULL, creationFlags, featureLevels, totalFeaturedLevels, D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
		if (SUCCEEDED(result))
		{
			break;
		}
		else if (FAILED(result))
		{
			return false;
		}
	}

	ID3D11Texture2D *backBufferTexture;
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&backBufferTexture);
	if (FAILED(result))
	{
		return false;
	}
	result = m_device->CreateRenderTargetView(backBufferTexture, NULL, &m_renderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	backBufferTexture->Release();
	backBufferTexture = NULL;

	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthBufferDesc.Width = m_screenWidth;
	depthBufferDesc.Height = m_screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;
	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the stencil state.
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
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
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState3D);
	if (FAILED(result))
	{
		return false;
	}
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState3D, 1);

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result))
	{
		return false;
	}
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	// Setup the raster description which will determine how and what polygons will be drawn.
	D3D11_RASTERIZER_DESC rasterDesc;
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
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterStateBackCull);
	if (FAILED(result))
	{
		return false;
	}
	m_deviceContext->RSSetState(m_rasterStateBackCull);

	rasterDesc.CullMode = D3D11_CULL_FRONT;
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterStateFrontCull);
	if (FAILED(result))
	{
		return false;
	}

	m_viewport.Width = (float)m_screenWidth;
	m_viewport.Height = (float)m_screenHeight;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;
	m_deviceContext->RSSetViewports(1, &m_viewport);

	float fieldOfView = XM_PIDIV4;
	float screenAspect = (float)m_screenWidth / (float)m_screenHeight;

	m_projectionMatrix = XMMatrixTranspose(XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth));
	m_identityMatrix = XMMatrixTranspose(XMMatrixIdentity());
	m_orthoMatrix = XMMatrixTranspose(XMMatrixOrthographicLH(static_cast<float>(m_screenWidth), static_cast<float>(m_screenHeight), -1, 1));

	// Clear the second depth stencil state before setting the parameters.
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));
	// Now create a second depth stencil state which turns off the Z buffer for 2D rendering.  The only difference is
	// that DepthEnable is set to false, all other parameters are the same as the other depth stencil state.
	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	result = m_device->CreateDepthStencilState(&depthDisabledStencilDesc, &m_depthStencilState2D);
	if (FAILED(result))
	{
		return false;
	}

	// Create an alpha enabled blend state description.
	D3D11_BLEND_DESC blendStateDescription;
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;
	result = m_device->CreateBlendState(&blendStateDescription, &m_alphaEnableBlendingState);
	if (FAILED(result))
	{
		return false;
	}

	blendStateDescription.RenderTarget[0].BlendEnable = FALSE;
	result = m_device->CreateBlendState(&blendStateDescription, &m_alphaDisableBlendingState);
	if (FAILED(result))
	{
		return false;
	}
	return true;
}

void D3D::Release()
{
	SAFE_RELEASE(m_alphaEnableBlendingState);
	SAFE_RELEASE(m_alphaDisableBlendingState);
	SAFE_RELEASE(m_swapChain);
	SAFE_RELEASE(m_depthStencilState2D);
	SAFE_RELEASE(m_rasterStateBackCull);
	SAFE_RELEASE(m_rasterStateFrontCull);
	SAFE_RELEASE(m_depthStencilView);
	SAFE_RELEASE(m_depthStencilState3D);
	SAFE_RELEASE(m_depthStencilBuffer);
	SAFE_RELEASE(m_renderTargetView);
	SAFE_RELEASE(m_deviceContext);
	SAFE_RELEASE(m_device);
	SAFE_RELEASE(m_swapChain);
}

void D3D::TurnAlphaBlendingOn()
{
	float blendFactor[4];
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;
	m_deviceContext->OMSetBlendState(m_alphaEnableBlendingState, blendFactor, 0xffffffff);
}

void D3D::TurnAlphaBlendingOff()
{
	float blendFactor[4];
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;
	m_deviceContext->OMSetBlendState(m_alphaDisableBlendingState, blendFactor, 0xffffffff);
}

void D3D::Clear(float red, float green, float blue, float alpha)
{
	float color[4];
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;
	UINT flags = D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL;
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, flags, 1.0f, 0);
}

void D3D::BeginScene()
{
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
}

void D3D::EndScene()
{
	if (m_vsyncEnabled)
	{
		m_swapChain->Present(1, 0);
	}
	else
	{
		m_swapChain->Present(0, 0);
	}
	ID3D11ShaderResourceView *const pSRV[1] = {NULL};
	m_deviceContext->PSSetShaderResources(1, 1, pSRV);
}

ID3D11Device *D3D::GetDevice()
{
	return m_device;
}

ID3D11DeviceContext *D3D::GetDeviceContext()
{
	return m_deviceContext;
}

const MAT4 &D3D::GetProjectionMatrix() const
{
	return m_projectionMatrix;
}

const MAT4 &D3D::GetIdentityMatrix() const
{
	return m_identityMatrix;
}

const MAT4 &D3D::GetOrthoMatrix() const
{
	return m_orthoMatrix;
}

void D3D::GetVideoCardInfo(char *cardName, int &memory)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
}

void D3D::TurnZBufferOn()
{
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState3D, 1);
}

void D3D::TurnZBufferOff()
{
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState2D, 1);
}

int D3D::GetScreenWidth() const
{
	return m_screenWidth;
}

int D3D::GetScreenHeight() const
{
	return m_screenHeight;
}

void D3D::SetBackBufferRenderTarget()
{
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
}

void D3D::ResetViewPort()
{
	m_deviceContext->RSSetViewports(1, &m_viewport);
}

void D3D::SetBackCulling()
{
	m_deviceContext->RSSetState(m_rasterStateBackCull);
}

void D3D::SetFrontCulling()
{
	m_deviceContext->RSSetState(m_rasterStateFrontCull);
}