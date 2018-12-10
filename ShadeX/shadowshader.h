#ifndef SE_SHADOWSHADERDX_H
#define SE_SHADOWSHADERDX_H

#include "shader.h"
#include "shadowmap.h"

SE_ALIGN16 class ShadowShader : public Shader {
private:
	struct LightBufferType {
		VEC4	ambientColor;
		VEC4	diffuseColor;
		VEC4	specularColor;
		VEC3	lightDirection;
		float	specularPower;
		VEC4	shadowTextureSize;
	};
	SE_ALIGN16 struct MatrixBufferType : public seAligned {
		MAT4	worldMatrix;
		MAT4	viewMatrix;
		MAT4	projectionMatrix;
		MAT4	lightVPMatrix;
	};
	struct CameraBufferType {
		VEC3	cameraPosition;
		float	padding;
	};

public:
	ShadowShader();
	ShadowShader(const ShadowShader&);
	~ShadowShader();

	bool					Render(ID3D11DeviceContext* devCont, UINT indCount, const MAT4& wMat, const Camera* camera, const Light* light, ID3D11ShaderResourceView* tex, ShadowMapBufferRenderer* shadowMap);

private:
	void					RenderShader(ID3D11DeviceContext*, UINT);
	bool					SetShaderParameters(ID3D11DeviceContext*, const MAT4&, const Camera* camera, const Light* light, ID3D11ShaderResourceView* tex, ShadowMapBufferRenderer* shadowMap);
	bool					InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void					ReleaseShader();

private:
	ID3D11Buffer*			m_cameraBuffer;
	ID3D11Buffer*			m_lightBuffer;
	ID3D11SamplerState*		m_sampleStateClamp;
};


#endif // !SE_LIGHTSHADERDX_H
