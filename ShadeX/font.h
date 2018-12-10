#ifndef SE_FONTDX_H
#define SE_FONTDX_H

#include "globals.h"
#include "d3d.h"
#include "texture.h"

class Font
{
  public:
	__inline static bool isNear(const float v1, const float v2, const float margin)
	{
		return fabs(v1 - v2) < margin;
	}

	struct VertexType
	{
		VEC3 position;
		VEC2 texture;

		VertexType(){};
		VertexType(const VEC3 &p, const VEC2 &u) : position(p), texture(u){};

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
			return false;
		};
	};

	struct FontType
	{
		char ch;
		float width, height, xOffset, yOffset, xAdvance;
		float top, bottom, left, right;
	};

	typedef map<int, FontType> FONTMAP;

  public:
	Font();
	Font(const Font &other);
	virtual ~Font();

	bool Initialize(ID3D11Device *, const WCHAR *textureFile, const WCHAR *dataFile);
	void Release();

	void BuildVertexArray(const char *sentence, float scale, float positionX, float positionY, PACKEDVECTOR &outPacked);

	ID3D11ShaderResourceView *GetTexture() const;

  private:
	bool LoadFontData(const WCHAR *fileName);
	bool LoadTexture(ID3D11Device *, const WCHAR *);
	void ReleaseFontData();
	void ReleaseTexture();

  private:
	Texture *m_texture;
	FONTMAP m_fontMap;
};

#endif // !SE_FONTDX_H
