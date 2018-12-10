#include "lightshader.h"

LightShader::LightShader() : Shader()
{
	m_lightBuffer = NULL;
	m_cameraBuffer = NULL;
	m_type = Shader::LightType;
}

LightShader::LightShader(const LightShader &other)
{
	UNREFERENCED_PARAMETER(other);
}

LightShader::~LightShader() {}

bool LightShader::Render(ID3D11DeviceContext *deviceContext, UINT indexCount, const MAT4 &worldMatrix, const Camera *camera, const Light *light, ID3D11ShaderResourceView *texture)
{
	bool result = SetShaderParameters(deviceContext, worldMatrix, camera, light, texture);
	if (!result)
	{
		return false;
	}
	RenderShader(deviceContext, indexCount);
	return true;
}

bool LightShader::InitializeShader(ID3D11Device *device, HWND hwnd, WCHAR *vsFilename, WCHAR *psFilename)
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
	result = CompileShaderFromFile(vsFilename, "LightVertexShader", "vs_5_0", &vertexShaderBuffer);
	if (FAILED(result))
	{
		MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		return false;
	}

	result = CompileShaderFromFile(psFilename, "LightPixelShader", "ps_5_0", &pixelShaderBuffer);
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

	D3D11_INPUT_ELEMENT_DESC polygonLayout[5];

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

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	polygonLayout[3].SemanticName = "TANGENT";
	polygonLayout[3].SemanticIndex = 0;
	polygonLayout[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[3].InputSlot = 0;
	polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[3].InstanceDataStepRate = 0;

	polygonLayout[4].SemanticName = "BINORMAL";
	polygonLayout[4].SemanticIndex = 0;
	polygonLayout[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[4].InputSlot = 0;
	polygonLayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[4].InstanceDataStepRate = 0;

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

	D3D11_BUFFER_DESC cameraBufferDesc;
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;
	result = device->CreateBuffer(&cameraBufferDesc, NULL, &m_cameraBuffer);
	if (FAILED(result))
	{
		return false;
	}

	D3D11_BUFFER_DESC lightBufferDesc;
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	result = device->CreateBuffer(&lightBufferDesc, NULL, &m_lightBuffer);
	if (FAILED(result))
	{
		return false;
	}

	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
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

void LightShader::ReleaseShader()
{
	SAFE_RELEASE(m_sampleStateWrap);
	SAFE_RELEASE(m_matrixBuffer);
	SAFE_RELEASE(m_lightBuffer);
	SAFE_RELEASE(m_cameraBuffer);
	SAFE_RELEASE(m_layout);
	SAFE_RELEASE(m_pixelShader);
	SAFE_RELEASE(m_vertexShader);
}

bool LightShader::SetShaderParameters(ID3D11DeviceContext *deviceContext, const MAT4 &worldMatrix, const Camera *camera, const Light *light, ID3D11ShaderResourceView *texture)
{

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	UINT bufferNumber = 0;
	HRESULT result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}
	MatrixBufferType *matrixPtr = (MatrixBufferType *)mappedResource.pData;
	matrixPtr->world = worldMatrix;
	matrixPtr->view = camera->GetViewMatrix();
	matrixPtr->projection = camera->GetProjectionMatrix();
	deviceContext->Unmap(m_matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	bufferNumber = 1;
	result = deviceContext->Map(m_cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}
	CameraBufferType *cameraPtr = (CameraBufferType *)mappedResource.pData;
	cameraPtr->cameraPosition = camera->GetPosition();
	cameraPtr->padding = 0;
	deviceContext->Unmap(m_cameraBuffer, 0);
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_cameraBuffer);

	bufferNumber = 2;
	result = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}
	LightBufferType *lightPtr = (LightBufferType *)mappedResource.pData;
	lightPtr->ambientColor = light->GetAmbientColor();
	lightPtr->diffuseColor = light->GetDiffuseColor();
	lightPtr->specularColor = light->GetSpecularColor();
	lightPtr->lightDirection = light->GetDirection();
	lightPtr->specularPower = light->GetSpecularPower();
	deviceContext->Unmap(m_lightBuffer, 0);
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);
	deviceContext->PSSetConstantBuffers(0, 1, &m_lightBuffer);

	deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
}
