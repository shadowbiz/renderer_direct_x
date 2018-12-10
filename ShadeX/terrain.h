#ifndef SE_TERRAIN_H
#define SE_TERRAIN_H

#include "globals.h"
#include "alignedbase.h"
#include "bitmaploader.h"
#include "model.h"

#define TEXTURE_REPEAT 8

SE_ALIGN16 class Terrain : public Model
{
  public:
	Terrain();
	Terrain(const Terrain &other);
	~Terrain();

	bool Initialize(ID3D11Device *device, const char *fileName);
	void Render(ID3D11DeviceContext *deviceContext);

  private:
	bool LoadHeightMap(const char *fileName);

	void CalculateFaceNormal(const VEC3 &a, const VEC3 &b, const VEC3 &c, VEC3 &outN) const;
	void CalculateUV(const float x, const float y, const float width, const float height, VEC2 &outUv) const;
	void NormalizeHeightMap();
	bool InitializeBuffers(ID3D11Device *device);
	void RenderBuffers(ID3D11DeviceContext *deviceContext);

  private:
	UINT m_terrainWidth;
	UINT m_terrainHeight;
	VEC3VECTOR m_heightMap;
};

#endif // !SE_TERRAIN_H
