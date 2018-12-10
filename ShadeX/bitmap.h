#ifndef SE_BITMAPDX_H
#define SE_BITMAPDX_H

#include "globals.h"
#include "d3d.h"
#include "texture.h"

class Bitmap
{
  private:
	struct VertexType
	{
		VEC3 position;
		VEC2 texture;
	};

  public:
	Bitmap();
	Bitmap(const Bitmap &);
	virtual ~Bitmap();

	bool Initialize(D3D *, const WCHAR *, int, int);
	void Release();
	bool Render(ID3D11DeviceContext *, int, int);

	UINT GetIndexCount() const;
	ID3D11ShaderResourceView *GetTexture() const;

  private:
	bool InitializeBuffers(ID3D11Device *);
	void ReleaseBuffers();
	bool UpdateBuffers(ID3D11DeviceContext *, int, int);
	void RenderBuffers(ID3D11DeviceContext *);

	bool LoadTexture(ID3D11Device *, const WCHAR *);
	void ReleaseTexture();

  private:
	ID3D11Buffer *m_vertexBuffer;
	ID3D11Buffer *m_indexBuffer;
	UINT m_vertexCount;
	UINT m_indexCount;
	Texture *m_texture;
	int m_screenWidth;
	int m_screenHeight;
	int m_bitmapWidth;
	int m_bitmapHeight;
	int m_previousPosX;
	int m_previousPosY;
	VertexType *m_vertices;
};

#endif