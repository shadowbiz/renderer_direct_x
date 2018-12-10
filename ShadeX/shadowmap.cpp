#include "shadowmap.h"

ShadowMapBufferRenderer::ShadowMapBufferRenderer()
{
	m_shadowMapBias = 0;
	m_shadowMapDepthView = NULL;
	m_shadowMapSRView = NULL;
	m_shadowMap = NULL;
}

ShadowMapBufferRenderer::ShadowMapBufferRenderer(const ShadowMapBufferRenderer &other)
{
	UNREFERENCED_PARAMETER(other);
}

ShadowMapBufferRenderer::~ShadowMapBufferRenderer()
{
}

bool ShadowMapBufferRenderer::Initialize(ID3D11Device *device, UINT width, UINT height)
{

	m_width = width;
	m_height = height;

	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	HRESULT result = device->CreateTexture2D(&texDesc, NULL, &m_shadowMap);
	if (FAILED(result))
	{
		return false;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Flags = 0;
	descDSV.Texture2D.MipSlice = 0;
	result = device->CreateDepthStencilView(m_shadowMap, &descDSV, &m_shadowMapDepthView);
	if (FAILED(result))
	{
		return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	result = device->CreateShaderResourceView(m_shadowMap, &srvDesc, &m_shadowMapSRView);
	if (FAILED(result))
	{
		return false;
	}
	m_shadowMapViewport.Width = (float)width;
	m_shadowMapViewport.Height = (float)height;
	m_shadowMapViewport.MinDepth = 0.0f;
	m_shadowMapViewport.MaxDepth = 1.0f;
	m_shadowMapViewport.TopLeftX = 0;
	m_shadowMapViewport.TopLeftY = 0;
	return true;
}

void ShadowMapBufferRenderer::Begin(ID3D11DeviceContext *deviceContext)
{
	UINT flags = D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL;
	ID3D11RenderTargetView *pNullRTV = NULL;
	deviceContext->RSSetViewports(1, &m_shadowMapViewport);
	deviceContext->OMSetRenderTargets(1, &pNullRTV, m_shadowMapDepthView);
	deviceContext->ClearDepthStencilView(m_shadowMapDepthView, flags, 1.0f, 0);
}

void ShadowMapBufferRenderer::End(ID3D11DeviceContext *deviceContext)
{
	// 	ID3D11RenderTargetView* pNullRTV = NULL;
	// 	deviceContext->OMSetRenderTargets(1, &pNullRTV, NULL);
}

void ShadowMapBufferRenderer::Release()
{
	SAFE_RELEASE(m_shadowMap);
	SAFE_RELEASE(m_shadowMapDepthView);
	SAFE_RELEASE(m_shadowMapSRView);
}

ID3D11ShaderResourceView *ShadowMapBufferRenderer::GetShaderResourceView() const
{
	return m_shadowMapSRView;
}

UINT ShadowMapBufferRenderer::GetWidth() const
{
	return m_width;
}

UINT ShadowMapBufferRenderer::GetHeight() const
{
	return m_height;
}