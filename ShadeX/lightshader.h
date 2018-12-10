#ifndef SE_LIGHTSHADERDX_H
#define SE_LIGHTSHADERDX_H

#include "shader.h"

SE_ALIGN16 class LightShader : public Shader
{
  private:
	struct LightBufferType
	{
		VEC4 ambientColor;
		VEC4 diffuseColor;
		VEC4 specularColor;
		VEC3 lightDirection;
		float specularPower;
	};

	struct CameraBufferType
	{
		VEC3 cameraPosition;
		float padding;
	};

  public:
	LightShader();
	LightShader(const LightShader &);
	~LightShader();

	bool Render(ID3D11DeviceContext *devCont, UINT indCount, const MAT4 &wMat, const Camera *camera, const Light *light, ID3D11ShaderResourceView *tex);

  private:
	bool SetShaderParameters(ID3D11DeviceContext *, const MAT4 &, const Camera *camera, const Light *light, ID3D11ShaderResourceView *tex);
	bool InitializeShader(ID3D11Device *, HWND, WCHAR *, WCHAR *);
	void ReleaseShader();

  private:
	ID3D11Buffer *m_cameraBuffer;
	ID3D11Buffer *m_lightBuffer;
};

#endif // !SE_LIGHTSHADERDX_H
