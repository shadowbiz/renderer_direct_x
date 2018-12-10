#ifndef SE_SHADERDX_H
#define SE_SHADERDX_H

#include "globals.h"
#include "alignedbase.h"
#include "camera.h"
#include "light.h"

SE_ALIGN16 class Shader : public seAligned
{
  public:
	enum ShaderType
	{
		DepthType,
		LightType,
		TextureType,
		ShadowType
	};

  protected:
	SE_ALIGN16 struct MatrixBufferType : public seAligned
	{
		MAT4 world;
		MAT4 view;
		MAT4 projection;
	};

  public:
	Shader();
	Shader(const Shader &other);
	virtual ~Shader();

  public:
	bool Initialize(ID3D11Device *, HWND);
	void Release();
	bool Render(ID3D11DeviceContext *devCont, UINT indCount, const MAT4 &wMat, const Camera *camera);
	bool Render(ID3D11DeviceContext *devCont, UINT indCount, const MAT4 &wMat, const Camera *camera, const Light *light, ID3D11ShaderResourceView *tex);
	bool Render(ID3D11DeviceContext *devCont, UINT indCount, const MAT4 &wMat, const Camera *camera, ID3D11ShaderResourceView *tex);
	bool Render(ID3D11DeviceContext *devCont, UINT indCount, const MAT4 &wMat, const MAT4 &vMat, const MAT4 &pMat, ID3D11ShaderResourceView *tex);

  protected:
	bool SetShaderParameters(ID3D11DeviceContext *, const MAT4 &, const Camera *camera);
	bool SetShaderParameters(ID3D11DeviceContext *, const MAT4 &, const Camera *camera, const Light *light, ID3D11ShaderResourceView *tex);
	bool SetShaderParameters(ID3D11DeviceContext *, const MAT4 &, const Camera *camera, ID3D11ShaderResourceView *);
	bool SetShaderParameters(ID3D11DeviceContext *, const MAT4 &, const MAT4 &, const MAT4 &, ID3D11ShaderResourceView *);
	virtual void RenderShader(ID3D11DeviceContext *, UINT);
	HRESULT CompileShaderFromFile(const WCHAR *fileName, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob **blobOut) const;
	void OutputShaderErrorMessage(ID3D10Blob *, HWND, WCHAR *);

  protected:
	virtual bool InitializeShader(ID3D11Device *, HWND, WCHAR *, WCHAR *) = 0;
	virtual void ReleaseShader() = 0;

  protected:
	ShaderType m_type;
	ID3D11VertexShader *m_vertexShader;
	ID3D11PixelShader *m_pixelShader;
	ID3D11InputLayout *m_layout;
	ID3D11Buffer *m_matrixBuffer;
	ID3D11SamplerState *m_sampleStateWrap;
};

#endif