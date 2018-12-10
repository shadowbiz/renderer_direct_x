#ifndef SE_DEPTHSHADER_H
#define SE_DEPTHSHADER_H

#include "shader.h"
#include "camera.h"

SE_ALIGN16 class DepthShader : public Shader
{
  public:
	DepthShader();
	DepthShader(const DepthShader &);
	~DepthShader();

	bool Render(ID3D11DeviceContext *devCont, UINT indCount, const MAT4 &wMat, const Camera *camera);
	bool Render(ID3D11DeviceContext *devCont, UINT indCount, const MAT4 &wMat, const MAT4 &vMat, const MAT4 &pMat);

  private:
	bool SetShaderParameters(ID3D11DeviceContext *, const MAT4 &wMat, const MAT4 &vMat, const MAT4 &pMat);
	bool InitializeShader(ID3D11Device *, HWND, WCHAR *, WCHAR *);
	void ReleaseShader();
	void RenderShader(ID3D11DeviceContext *deviceContext, UINT indexCount);
};

#endif // !SE_DEPTHSHADER_H
