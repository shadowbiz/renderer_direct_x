#ifndef SE_SCENEDX_H
#define SE_SCENEDX_H

#include "globals.h"
#include "alignedbase.h"
#include "d3d.h"
#include "input.h"
#include "textureshader.h"
#include "lightshader.h"
#include "depthshader.h"
#include "shadowshader.h"
#include "light.h"
#include "camera.h"
#include "gameobject.h"
#include "terrain.h"

SE_ALIGN16 class Scene : public seAligned
{
  public:
	Scene();
	Scene(const Scene &other);
	~Scene();

	bool Initialize(D3D *d3D, HWND hwnd);
	void Release();

	bool Update(float deltaTime, const Input *input);
	bool Render(ShadowShader *shader, ShadowMapBufferRenderer *shadowMap);
	bool ShadowPass(DepthShader *depthShader);

	Camera *GetCamera() const;

  private:
	D3D *m_D3D;
	Camera *m_camera;
	Light *m_light;
	Model *m_model;
	Model *m_plane;
	Terrain *m_terrain;
	GameObject *m_gameObject1;
	GameObject *m_gameObject3;
	GameObject *m_gameObject4;
	GOPTRVECTOR m_gameObjects;
};

#endif // !SE_SCENEDX_H
