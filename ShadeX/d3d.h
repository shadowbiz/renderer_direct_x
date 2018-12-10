#ifndef SE_D3D_H
#define SE_D3D_H

#include "globals.h"
#include "alignedbase.h"

SE_ALIGN16 class D3D : public seAligned
{
  public:
	D3D();
	D3D(const D3D &);
	~D3D();

	bool Initialize(HWND, bool, bool, float, float);
	void Release();

	void Clear(float, float, float, float);
	void BeginScene();
	void EndScene();

	ID3D11Device *GetDevice();
	ID3D11DeviceContext *GetDeviceContext();

	const MAT4 &GetProjectionMatrix() const;
	const MAT4 &GetIdentityMatrix() const;
	const MAT4 &GetOrthoMatrix() const;

	void GetVideoCardInfo(char *, int &);

	void TurnZBufferOn();
	void TurnZBufferOff();

	void TurnAlphaBlendingOn();
	void TurnAlphaBlendingOff();

	void SetBackCulling();
	void SetFrontCulling();

	int GetScreenWidth() const;
	int GetScreenHeight() const;

	//ID3D11DepthStencilView* GetDepthStencilView();
	void SetBackBufferRenderTarget();
	void ResetViewPort();

  private:
	bool m_vsyncEnabled;
	int m_screenWidth;
	int m_screenHeight;
	int m_videoCardMemory;
	char m_videoCardDescription[128];
	IDXGISwapChain *m_swapChain;
	ID3D11Device *m_device;
	ID3D11DeviceContext *m_deviceContext;
	ID3D11RenderTargetView *m_renderTargetView;
	ID3D11Texture2D *m_depthStencilBuffer;
	ID3D11DepthStencilState *m_depthStencilState3D;
	ID3D11DepthStencilView *m_depthStencilView;
	ID3D11RasterizerState *m_rasterStateBackCull;
	ID3D11RasterizerState *m_rasterStateFrontCull;
	MAT4 m_projectionMatrix;
	MAT4 m_identityMatrix;
	MAT4 m_orthoMatrix;
	ID3D11DepthStencilState *m_depthStencilState2D;
	ID3D11BlendState *m_alphaEnableBlendingState;
	ID3D11BlendState *m_alphaDisableBlendingState;
	D3D11_VIEWPORT m_viewport;
};

#endif