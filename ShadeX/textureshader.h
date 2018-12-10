#ifndef SE_TEXTURESHADERDX_H
#define SE_TEXTURESHADERDX_H

#include "shader.h"

SE_ALIGN16 class TextureShader : public Shader
{
  public:
	TextureShader();
	TextureShader(const TextureShader &);
	~TextureShader();

	bool Render(ID3D11DeviceContext *devCont, UINT indCount, const MAT4 &wMat, const Camera *camera, ID3D11ShaderResourceView *tex);
	bool Render(ID3D11DeviceContext *devCont, UINT indCount, const MAT4 &wMat, const MAT4 &vMat, const MAT4 &pMat, ID3D11ShaderResourceView *tex);

  private:
	bool SetShaderParameters(ID3D11DeviceContext *, const MAT4 &, const MAT4 &, const MAT4 &, ID3D11ShaderResourceView *);
	bool SetShaderParameters(ID3D11DeviceContext *, const MAT4 &, const Camera *camera, ID3D11ShaderResourceView *);
	bool InitializeShader(ID3D11Device *, HWND, WCHAR *, WCHAR *);
	void ReleaseShader();
};

#endif