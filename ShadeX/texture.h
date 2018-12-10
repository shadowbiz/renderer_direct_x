#ifndef SE_TEXTUREDX_H
#define SE_TEXTUREDX_H

#include "globals.h"
#include "ddsloader.h"

class Texture
{
  public:
	Texture();
	Texture(const Texture &);
	~Texture();

	bool Initialize(ID3D11Device *, const WCHAR *);
	void Release();

	ID3D11ShaderResourceView *GetTexture() const;

  private:
	ID3D11ShaderResourceView *m_texture;
};

#endif