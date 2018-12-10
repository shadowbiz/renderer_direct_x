#include "depthshader.h"

DepthShader::DepthShader() : Shader()
{
	m_type = Shader::DepthType;
}

DepthShader::DepthShader(const DepthShader &other)
{
	UNREFERENCED_PARAMETER(other);
}

DepthShader::~DepthShader() {}

bool DepthShader::Render(ID3D11DeviceContext *deviceContext, UINT indexCount, const MAT4 &worldMatrix, const Camera *camera)
{
	bool result = SetShaderParameters(deviceContext, worldMatrix, camera->GetViewMatrix(), camera->GetProjectionMatrix());
	if (!result)
	{
		return false;
	}
	RenderShader(deviceContext, indexCount);
	return true;
}

bool DepthShader::Render(ID3D11DeviceContext *deviceContext, UINT indexCount, const MAT4 &worldMatrix, const MAT4 &vMatrix, const MAT4 &pMatrix)
{
	bool result = SetShaderParameters(deviceContext, worldMatrix, vMatrix, pMatrix);
	if (!result)
	{
		return false;
	}
	RenderShader(deviceContext, indexCount);
	return true;
}

bool DepthShader::InitializeShader(ID3D11Device *device, HWND hwnd, WCHAR *vsFilename, WCHAR *psFilename)
{
	ID3DBlob *vertexShaderBuffer = NULL;
	HRESULT result = S_OK;

#if USE_PRECOMPILED_SHADERS
	result = D3DReadFileToBlob(vsFilename, &vertexShaderBuffer);
	if (FAILED(result))
	{
		MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		return false;
	}
#else
	result = CompileShaderFromFile(vsFilename, "DepthVertexShader", "vs_5_0", &vertexShaderBuffer);
	if (FAILED(result))
	{
		MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		return false;
	}
#endif
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC polygonLayout[1];
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	UINT numElements = ARRAYSIZE(polygonLayout);

	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{
		return false;
	}
	SAFE_RELEASE(vertexShaderBuffer);

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
	return true;
}

void DepthShader::ReleaseShader()
{
	SAFE_RELEASE(m_sampleStateWrap);
	SAFE_RELEASE(m_matrixBuffer);
	SAFE_RELEASE(m_layout);
	SAFE_RELEASE(m_pixelShader);
	SAFE_RELEASE(m_vertexShader);
}

bool DepthShader::SetShaderParameters(ID3D11DeviceContext *deviceContext, const MAT4 &wMat, const MAT4 &vMat, const MAT4 &pMat)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	UINT bufferNumber = 0;
	HRESULT result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}
	MatrixBufferType *matrixPtr = (MatrixBufferType *)mappedResource.pData;
	matrixPtr->world = wMat;
	matrixPtr->view = vMat;
	matrixPtr->projection = pMat;
	deviceContext->Unmap(m_matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	return true;
}

void DepthShader::RenderShader(ID3D11DeviceContext *deviceContext, UINT indexCount)
{
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(NULL, NULL, 0);
	deviceContext->IASetInputLayout(m_layout);
	deviceContext->DrawIndexed(indexCount, 0, 0);
}
