////////////////////////////////////////////////////////////////////////////////
// Filename: textureclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "texture.h"

Texture::Texture()
{
	m_texture = NULL;
}

Texture::Texture(const Texture &other)
{
	UNREFERENCED_PARAMETER(other);
}
Texture::~Texture() {}

bool Texture::Initialize(ID3D11Device *device, const WCHAR *filename)
{
	HRESULT result = CreateDDSTextureFromFile(device, filename, NULL, &m_texture);
	if (FAILED(result))
	{
		return false;
	}
	return true;
}

void Texture::Release()
{
	SAFE_RELEASE(m_texture);
}

ID3D11ShaderResourceView *Texture::GetTexture() const
{
	return m_texture;
}