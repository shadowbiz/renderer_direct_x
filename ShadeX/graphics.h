#ifndef SE_GRAPHICS_H
#define SE_GRAPHICS_H

#include "globals.h"
#include "d3d.h"
#include "input.h"
#include "texture.h"
#include "textureshader.h"
#include "lightshader.h"
#include "depthshader.h"
#include "scene.h"
#include "gui.h"
#include "rendertexture.h"
#include "debugwindow.h"
#include "shadowmap.h"

#define FULL_SCREEN false
#define VSYNC_ENABLED true

const float SCREEN_DEPTH = 300.0f;
const float SCREEN_NEAR = 1.0f;

class Graphics
{
  public:
	Graphics();
	Graphics(const Graphics &);
	~Graphics();

	bool Initialize(HWND);
	void Release();
	bool Update(float deltaTime, Input *, int, int);
	bool Render();

  private:
	bool RenderToTexture();
	bool RenderShadowPass();
	bool RenderNormalPass();
	bool RenderGui();

  private:
	D3D *m_D3D;
	Gui *m_gui;
	RenderTexture *m_renderTexture;
	DebugWindow *m_debugWindow;
	Scene *m_scene;
	TextureShader *m_textureShader;
	ShadowShader *m_shadowShader;
	LightShader *m_lightShader;
	DepthShader *m_depthShader;
	Texture *m_texture;
	ShadowMapBufferRenderer *m_shadowMap;
};

#endif