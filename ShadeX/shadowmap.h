#ifndef SE_SHADOWMAPDX_H
#define SE_SHADOWMAPDX_H

#include "globals.h"
#include "d3d.h"

class ShadowMapBufferRenderer
{
  public:
	ShadowMapBufferRenderer();
	ShadowMapBufferRenderer(const ShadowMapBufferRenderer &other);
	~ShadowMapBufferRenderer();

	bool Initialize(ID3D11Device *device, UINT width, UINT height);
	void Release();

	void Begin(ID3D11DeviceContext *deviceContext);
	void End(ID3D11DeviceContext *deviceContext);

	ID3D11ShaderResourceView *GetShaderResourceView() const;
	UINT GetWidth() const;
	UINT GetHeight() const;

  private:
	UINT m_width;
	UINT m_height;
	ID3D11Texture2D *m_shadowMap;
	ID3D11DepthStencilView *m_shadowMapDepthView;
	ID3D11ShaderResourceView *m_shadowMapSRView;
	D3D11_VIEWPORT m_shadowMapViewport;
	float m_shadowMapBias;
};

#endif // !SE_SHADOWMAPDX_H
