#ifndef SE_RENDERTEXTURE_H
#define SE_RENDERTEXTURE_H

#include "globals.h"
#include "alignedbase.h"
SE_ALIGN16 class RenderTexture : public seAligned
{
  public:
	RenderTexture();
	RenderTexture(const RenderTexture &other);
	~RenderTexture();

	bool Initialize(ID3D11Device *device, int width, int height, int nearDepth, int farDepth);
	void Release();

	void SetRenderTarget(ID3D11DeviceContext *);
	void ClearRenderTarget(ID3D11DeviceContext *, const VEC4 &color);
	ID3D11ShaderResourceView *GetShaderResourceView();

  private:
	ID3D11Texture2D *m_renderTargetTexture;
	ID3D11RenderTargetView *m_renderTargetView;
	ID3D11ShaderResourceView *m_shaderResourceView;
	ID3D11Texture2D *m_depthStencilBuffer;
	ID3D11DepthStencilView *m_depthStencilView;
	D3D11_VIEWPORT m_viewport;
	MAT4 m_projectionMatrix;
	MAT4 m_orthoMatrix;
};

#endif // !SE_RENDERTEXTURE_H
