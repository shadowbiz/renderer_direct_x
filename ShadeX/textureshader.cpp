#include "textureshader.h"

TextureShader::TextureShader() : Shader()
{
	m_type = Shader::TextureType;
}

TextureShader::TextureShader(const TextureShader &other)
{
	UNREFERENCED_PARAMETER(other);
}

TextureShader::~TextureShader() {}

bool TextureShader::Render(ID3D11DeviceContext *deviceContext, UINT indexCount, const MAT4 &worldMatrix, const MAT4 &viewMatrix, const MAT4 &projectionMatrix, ID3D11ShaderResourceView *texture)
{
	bool result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture);
	if (!result)
	{
		return false;
	}
	RenderShader(deviceContext, indexCount);
	return true;
}

bool TextureShader::Render(ID3D11DeviceContext *deviceContext, UINT indexCount, const MAT4 &wMat, const Camera *camera, ID3D11ShaderResourceView *texture)
{
	return Render(deviceContext, indexCount, wMat, camera->GetViewMatrix(), camera->GetProjectionMatrix(), texture);
}

bool TextureShader::InitializeShader(ID3D11Device *device, HWND hwnd, WCHAR *vsFilename, WCHAR *psFilename)
{

	ID3DBlob *vertexShaderBuffer = NULL;
	ID3DBlob *pixelShaderBuffer = NULL;

	HRESULT result = S_OK;

#if USE_PRECOMPILED_SHADERS
	result = D3DReadFileToBlob(vsFilename, &vertexShaderBuffer);
	if (FAILED(result))
	{
		MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		return false;
	}

	result = D3DReadFileToBlob(psFilename, &pixelShaderBuffer);
	if (FAILED(result))
	{
		MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		return false;
	}
#else
	result = CompileShaderFromFile(vsFilename, "TextureVertexShader", "vs_5_0", &vertexShaderBuffer);
	if (FAILED(result))
	{
		MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		return false;
	}

	result = CompileShaderFromFile(psFilename, "TexturePixelShader", "ps_5_0", &pixelShaderBuffer);
	if (FAILED(result))
	{
		MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		return false;
	}
#endif

	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	UINT numElements = ARRAYSIZE(polygonLayout);

	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{
		return false;
	}

	vertexShaderBuffer->Release();
	vertexShaderBuffer = NULL;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = NULL;

	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	result = device->CreateSamplerState(&samplerDesc, &m_sampleStateWrap);
	if (FAILED(result))
	{
		return false;
	}
	return true;
}

void TextureShader::ReleaseShader()
{
	SAFE_RELEASE(m_sampleStateWrap);
	SAFE_RELEASE(m_matrixBuffer);
	SAFE_RELEASE(m_layout);
	SAFE_RELEASE(m_pixelShader);
	SAFE_RELEASE(m_vertexShader);
}

bool TextureShader::SetShaderParameters(ID3D11DeviceContext *deviceContext, const MAT4 &worldMatrix, const MAT4 &viewMatrix, const MAT4 &projectionMatrix, ID3D11ShaderResourceView *texture)
{

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	MatrixBufferType *dataPtr = (MatrixBufferType *)mappedResource.pData;

	dataPtr->world = (worldMatrix);
	dataPtr->view = (viewMatrix);
	dataPtr->projection = (projectionMatrix);

	deviceContext->Unmap(m_matrixBuffer, 0);

	UINT bufferNumber = 0;

	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
	deviceContext->PSSetShaderResources(0, 1, &texture);
	return true;
}

bool TextureShader::SetShaderParameters(ID3D11DeviceContext *deviceContext, const MAT4 &worldMatrix, const Camera *camera, ID3D11ShaderResourceView *texture)
{
	return SetShaderParameters(deviceContext, worldMatrix, camera->GetViewMatrix(), camera->GetProjectionMatrix(), texture);
}