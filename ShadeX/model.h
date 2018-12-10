#ifndef SE_MODELDX_H
#define SE_MODELDX_H

#include "globals.h"

class Model
{
  public:
	enum ModelType
	{
		ModelPlane,
		ModelCube
	};

  protected:
	__inline static bool isNear(const float v1, const float v2, const float margin)
	{
		return fabs(v1 - v2) < margin;
	}
	struct VertexType
	{
		VEC3 position;
		VEC2 texture;
		VEC3 normal;
		VEC3 tangent;
		VEC3 binormal;

		VertexType(){};
		VertexType(const VEC3 &p, const VEC2 &u, const VEC3 &n)
			: position(p), texture(u), normal(n){};

		__inline bool operator()(const VertexType &a, const VertexType &b) const
		{
			if (!isNear(a.position.x, b.position.x, 0.001f))
				return a.position.x < b.position.x;
			if (!isNear(a.position.y, b.position.y, 0.001f))
				return a.position.y < b.position.y;
			if (!isNear(a.position.z, b.position.z, 0.001f))
				return a.position.z < b.position.z;
			if (!isNear(a.texture.x, b.texture.x, 0.1f))
				return a.texture.x < b.texture.x;
			if (!isNear(a.texture.y, b.texture.y, 0.1f))
				return a.texture.y < b.texture.y;
			if (!isNear(a.normal.x, b.normal.x, 0.3f))
				return a.normal.x < b.normal.x;
			if (!isNear(a.normal.y, b.normal.y, 0.3f))
				return a.normal.y < b.normal.y;
			if (!isNear(a.normal.z, b.normal.z, 0.3f))
				return a.normal.z < b.normal.z;
			return false;
		}

		__inline bool operator<(const VertexType &b) const
		{
			if (!isNear(position.x, b.position.x, 0.001f))
				return position.x < b.position.x;
			if (!isNear(position.y, b.position.y, 0.001f))
				return position.y < b.position.y;
			if (!isNear(position.z, b.position.z, 0.001f))
				return position.z < b.position.z;
			if (!isNear(texture.x, b.texture.x, 0.1f))
				return texture.x < b.texture.x;
			if (!isNear(texture.y, b.texture.y, 0.1f))
				return texture.y < b.texture.y;
			if (!isNear(normal.x, b.normal.x, 0.3f))
				return normal.x < b.normal.x;
			if (!isNear(normal.y, b.normal.y, 0.3f))
				return normal.y < b.normal.y;
			if (!isNear(normal.z, b.normal.z, 0.3f))
				return normal.z < b.normal.z;
			return false;
		};
	};

  public:
	Model();
	Model(const Model &);
	~Model();

	bool Initialize(ID3D11Device *, HWND hwnd, ModelType type);
	bool Initialize(ID3D11Device *, HWND hwnd, char *);
	virtual void Release();
	void Render(ID3D11DeviceContext *);

	UINT GetIndexCount() const;

  protected:
	virtual bool InitializeBuffers(ID3D11Device *);
	void ReleaseBuffers();
	void RenderBuffers(ID3D11DeviceContext *);

	bool LoadModel(char *);
	bool LoadModel(ModelType type);
	void ReleaseModel();

	bool GetSimiliarIndex(const VertexType &packed, VERTEXMAP &vertixes, DWORD &index);
	bool Optimize(const VEC3VECTOR &oldVertices, const VEC3VECTOR &oldNormals, const VEC2VECTOR &oldUvs);
	void CalculateTangent(const VEC3 &normal, VEC3 &tangent, VEC3 &bitangent);

  protected:
	ID3D11Buffer *m_vertexBuffer;
	ID3D11Buffer *m_indexBuffer;
	UINT m_vertexCount;
	UINT m_indexCount;
	VertexType *m_model;
	DWORD *m_indices;
};

#endif