#include "graphics.h"

Graphics::Graphics()
{
	m_D3D = NULL;
	m_textureShader = NULL;
	m_lightShader = NULL;
	m_depthShader = NULL;
	m_shadowShader = NULL;
	m_texture = NULL;
	m_gui = NULL;
	m_renderTexture = NULL;
	m_debugWindow = NULL;
	m_shadowMap = NULL;
	m_scene = NULL;
}

Graphics::Graphics(const Graphics &other)
{
	UNREFERENCED_PARAMETER(other);
}

Graphics::~Graphics()
{
}

bool Graphics::Initialize(HWND hwnd)
{
	m_D3D = new D3D;
	if (!m_D3D)
	{
		return false;
	}
	bool result = m_D3D->Initialize(hwnd, VSYNC_ENABLED, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	m_textureShader = new TextureShader;
	if (!m_textureShader)
	{
		return false;
	}
	result = m_textureShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}
	m_lightShader = new LightShader;
	if (!m_lightShader)
	{
		return false;
	}
	result = m_lightShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Light Shader.", L"Error", MB_OK);
		return false;
	}
	m_depthShader = new DepthShader;
	if (!m_depthShader)
	{
		return false;
	}
	result = m_depthShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Depth Shader.", L"Error", MB_OK);
		return false;
	}
	m_shadowShader = new ShadowShader;
	if (!m_shadowShader)
	{
		return false;
	}
	result = m_shadowShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Shadow Shader.", L"Error", MB_OK);
		return false;
	}
	m_scene = new Scene;
	if (!m_scene)
	{
		return false;
	}
	result = m_scene->Initialize(m_D3D, hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Scene.", L"Error", MB_OK);
		return false;
	}
	m_gui = new Gui;
	if (!m_gui)
	{
		return false;
	}
	result = m_gui->Initialize(m_D3D, hwnd, m_textureShader);
	if (!result)
	{
		return false;
	}

	m_renderTexture = new RenderTexture;
	if (!m_renderTexture)
	{
		return false;
	}
	//result = m_renderTexture->Initialize(m_D3D->GetDevice(), m_D3D->GetScreenWidth(), m_D3D->GetScreenHeight(), SCREEN_NEAR, SCREEN_DEPTH);
	result = m_renderTexture->Initialize(m_D3D->GetDevice(), 1024, 1024, (int)SCREEN_NEAR, (int)SCREEN_DEPTH);
	if (!result)
	{
		return false;
	}

	m_debugWindow = new DebugWindow;
	if (!m_debugWindow)
	{
		return false;
	}
	result = m_debugWindow->Initialize(m_D3D, 200, 200);
	if (!result)
	{
		return false;
	}
	m_shadowMap = new ShadowMapBufferRenderer;
	if (!m_shadowMap)
	{
		return false;
	}
	result = m_shadowMap->Initialize(m_D3D->GetDevice(), m_D3D->GetScreenWidth(), m_D3D->GetScreenHeight());
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize ShadowMap.", L"Error", MB_OK);
		return false;
	}
	return true;
}

void Graphics::Release()
{
	SAFE_RELEASE_DELETE(m_shadowMap);
	SAFE_RELEASE_DELETE(m_debugWindow);
	SAFE_RELEASE_DELETE(m_renderTexture);
	SAFE_RELEASE_DELETE(m_gui);
	SAFE_RELEASE_DELETE(m_scene);
	SAFE_RELEASE_DELETE(m_textureShader);
	SAFE_RELEASE_DELETE(m_depthShader);
	SAFE_RELEASE_DELETE(m_shadowShader);
	SAFE_RELEASE_DELETE(m_D3D);
}

bool Graphics::Update(float deltaTime, Input *input, int fps, int cpuUssage)
{
	m_scene->Update(deltaTime, input);
	m_gui->Update(deltaTime, fps, cpuUssage);
	return true;
}

bool Graphics::RenderGui()
{
	m_D3D->TurnZBufferOff();
	m_D3D->TurnAlphaBlendingOn();

	bool result = m_gui->Render();
	if (!result)
	{
		return false;
	}
	m_D3D->TurnZBufferOn();
	m_debugWindow->Render(m_D3D->GetDeviceContext(), 30, 110);
	m_textureShader->Render(m_D3D->GetDeviceContext(), m_debugWindow->GetIndexCount(), m_D3D->GetIdentityMatrix(), m_D3D->GetIdentityMatrix(), m_D3D->GetOrthoMatrix(), m_shadowMap->GetShaderResourceView());
	m_D3D->TurnAlphaBlendingOff();
	return true;
}

bool Graphics::RenderToTexture()
{
	m_renderTexture->SetRenderTarget(m_D3D->GetDeviceContext());
	m_renderTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), VEC4(0, 0, 0, 1));
	m_D3D->SetBackBufferRenderTarget();
	return true;
}

bool Graphics::RenderShadowPass()
{
	m_D3D->SetFrontCulling();
	m_shadowMap->Begin(m_D3D->GetDeviceContext());
	bool result = m_scene->ShadowPass(m_depthShader);
	if (!result)
	{
		return false;
	}
	m_shadowMap->End(m_D3D->GetDeviceContext());
	m_D3D->ResetViewPort();
	m_D3D->SetBackCulling();
	return true;
}

bool Graphics::RenderNormalPass()
{
	bool result = m_scene->Render(m_shadowShader, m_shadowMap);
	if (!result)
	{
		return false;
	}
	return true;
}

bool Graphics::Render()
{
	bool result;
	m_D3D->Clear(0.2f, 0.2f, 0.2f, 1.0f);
	result = RenderShadowPass();
	if (!result)
	{
		return false;
	}
	m_D3D->BeginScene();
	result = RenderNormalPass();
	if (!result)
	{
		return false;
	}
	result = RenderGui();
	if (!result)
	{
		return false;
	}
	m_D3D->EndScene();
	return true;
}