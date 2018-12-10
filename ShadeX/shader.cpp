#include "shader.h"

Shader::Shader()
{
	m_vertexShader = NULL;
	m_pixelShader = NULL;
	m_layout = NULL;
	m_matrixBuffer = NULL;
	m_sampleStateWrap = NULL;
}

Shader::Shader(const Shader &other)
{
	UNREFERENCED_PARAMETER(other);
}

Shader::~Shader()
{
}

bool Shader::Initialize(ID3D11Device *device, HWND hwnd)
{
	WCHAR *vFilename;
	WCHAR *pFilename;
	switch (m_type)
	{
	case Shader::DepthType:
	{
#if USE_PRECOMPILED_SHADERS
		vFilename = L"data//depthvertex.cso";
		pFilename = NULL; //L"depthpixel.cso";
#else
		vFilename = L"depthvertex.hlsl";
		pFilename = NULL; //L"depthpixel.hlsl";
#endif
	}
	break;
	case Shader::LightType:
	{
#if USE_PRECOMPILED_SHADERS
		vFilename = L"data//lightvertex.cso";
		pFilename = L"data//lightpixel.cso";
#else
		vFilename = L"lightvertex.hlsl";
		pFilename = L"lightpixel.hlsl";
#endif
	}
	break;
	case Shader::ShadowType:
	{
#if USE_PRECOMPILED_SHADERS
		vFilename = L"data//shadowvertex.cso";
		pFilename = L"data//shadowpixel.cso";
#else
		vFilename = L"shadowvertex.hlsl";
		pFilename = L"shadowpixel.hlsl";
#endif
	}
	break;
	default:
	{
#if USE_PRECOMPILED_SHADERS
		vFilename = L"data//texturevertex.cso";
		pFilename = L"data//texturepixel.cso";
#else
		vFilename = L"texturevertex.hlsl";
		pFilename = L"texturepixel.hlsl";
#endif
	}
	break;
	}
	bool result = InitializeShader(device, hwnd, vFilename, pFilename);
	if (!result)
	{
		return false;
	}
	return true;
}

void Shader::Release()
{
	ReleaseShader();
}

void Shader::RenderShader(ID3D11DeviceContext *deviceContext, UINT indexCount)
{
	deviceContext->IASetInputLayout(m_layout);
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);
	deviceContext->PSSetSamplers(0, 1, &m_sampleStateWrap);
	deviceContext->DrawIndexed(indexCount, 0, 0);
}

void Shader::OutputShaderErrorMessage(ID3DBlob *errorMessage, HWND hwnd, WCHAR *shaderFilename)
{
	char *compileErrors = (char *)(errorMessage->GetBufferPointer());
	DWORD bufferSize = errorMessage->GetBufferSize();
	ofstream fout("shader-error.txt");
	for (DWORD i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}
	fout.close();
	errorMessage->Release();
	errorMessage = NULL;
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);
}

HRESULT Shader::CompileShaderFromFile(const WCHAR *fileName, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob **blobOut) const
{
	HRESULT result = S_OK;
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif
	ID3DBlob *pErrorBlob;
	result = D3DCompileFromFile(fileName, NULL, NULL, entryPoint, shaderModel, dwShaderFlags, 0, blobOut, &pErrorBlob);
	if (FAILED(result))
	{
		if (pErrorBlob != NULL)
		{
			OutputDebugStringA((char *)pErrorBlob->GetBufferPointer());
		}
		if (pErrorBlob)
		{
			pErrorBlob->Release();
		}
		return result;
	}
	if (pErrorBlob)
	{
		pErrorBlob->Release();
	}
	return S_OK;
}