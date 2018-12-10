#ifndef SE_DEBUGWINDOW_H
#define SE_DEBUGWINDOW_H

#include "globals.h"
#include "d3d.h"

class DebugWindow
{
  private:
	struct VertexType
	{
		VEC3 position;
		VEC2 texture;
	};

  public:
	DebugWindow();
	DebugWindow(const DebugWindow &);
	virtual ~DebugWindow();

	bool Initialize(D3D *, int, int);
	void Release();
	bool Render(ID3D11DeviceContext *, int, int);

	UINT GetIndexCount() const;

  private:
	bool InitializeBuffers(ID3D11Device *);
	void ReleaseBuffers();
	bool UpdateBuffers(ID3D11DeviceContext *, int, int);
	void RenderBuffers(ID3D11DeviceContext *);

  private:
	ID3D11Buffer *m_vertexBuffer;
	ID3D11Buffer *m_indexBuffer;
	UINT m_vertexCount;
	UINT m_indexCount;
	int m_screenWidth;
	int m_screenHeight;
	int m_bitmapWidth;
	int m_bitmapHeight;
	int m_previousPosX;
	int m_previousPosY;
	VertexType *m_vertices;
};

#endif // !SE_DEBUGWINDOW_H
