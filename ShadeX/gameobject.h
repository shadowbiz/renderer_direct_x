#ifndef SE_GAMEOBJECTDX_H
#define SE_GAMEOBJECTDX_H

#include "globals.h"
#include "alignedbase.h"
#include "model.h"
#include "camera.h"
#include "light.h"
#include "lightshader.h"
#include "texture.h"

SE_ALIGN16 class GameObject : public seAligned
{
  public:
	GameObject();
	GameObject(const GameObject &other);
	~GameObject();

	bool Initialize(ID3D11Device *device, HWND hwnd, Model *, Texture *texture);
	bool Initialize(ID3D11Device *device, HWND hwnd, Model *, const WCHAR *texture);
	void Release();

	void Render(ID3D11DeviceContext *devCont);
	void Update(float deltaTime);

	void SetPosition(float x, float y, float z);
	void SetPosition(const VEC3 &position);
	void SetPosition(const XMVECTOR &position);

	void SetRotation(float x, float y, float z);
	void SetRotation(const VEC3 &position);
	void SetRotation(const XMVECTOR &position);

	const XMMATRIX &GetWorldMatrix() const;
	ID3D11ShaderResourceView *GetTexture() const;
	UINT GetIndexCount() const;

  private:
	bool LoadTexture(ID3D11Device *, const WCHAR *);
	void ReleaseTexture();
	void UpdateMatrices();

  private:
	Model *m_model;
	XMMATRIX m_worldMatrix;
	XMMATRIX m_translationMatrix;
	XMMATRIX m_rotationMatrix;
	XMMATRIX m_scaleMatrix;

	XMVECTOR m_position;
	XMVECTOR m_rotation;

	bool m_hasChanged;

	Texture *m_texture;
	bool m_textureCreated;
};

#define GOPTRVECTOR vector<GameObject *>

#endif // !SE_GAMEOBJECTDX_H
