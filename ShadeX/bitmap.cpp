#include "bitmap.h"

Bitmap::Bitmap()
{
	m_vertexBuffer = NULL;
	m_indexBuffer = NULL;
	m_texture = NULL;
	m_vertices = NULL;
}

Bitmap::Bitmap(const Bitmap &other)
{
	UNREFERENCED_PARAMETER(other);
}

Bitmap::~Bitmap()
{
}

bool Bitmap::Initialize(D3D *d3D, const WCHAR *textureFilename, int bitmapWidth, int bitmapHeight)
{
	m_screenWidth = d3D->GetScreenWidth();
	m_screenHeight = d3D->GetScreenHeight();

	m_bitmapWidth = bitmapWidth;
	m_bitmapHeight = bitmapHeight;

	m_previousPosX = -1;
	m_previousPosY = -1;

	bool result = InitializeBuffers(d3D->GetDevice());
	if (!result)
	{
		return false;
	}
	result = LoadTexture(d3D->GetDevice(), textureFilename);
	if (!result)
	{
		return false;
	}
	return true;
}

void Bitmap::Release()
{
	SAFE_DELETE_ARRAY(m_vertices);
	ReleaseTexture();
	ReleaseBuffers();
}

bool Bitmap::Render(ID3D11DeviceContext *deviceContext, int positionX, int positionY)
{
	bool result = UpdateBuffers(deviceContext, positionX, positionY);
	if (!result)
	{
		return false;
	}
	RenderBuffers(deviceContext);
	return true;
}

UINT Bitmap::GetIndexCount() const
{
	return m_indexCount;
}

ID3D11ShaderResourceView *Bitmap::GetTexture() const
{
	return m_texture->GetTexture();
}

bool Bitmap::InitializeBuffers(ID3D11Device *device)
{
	m_vertexCount = 4;
	m_indexCount = 6;
	const DWORD indices[6] = {0, 1, 2, 2, 1, 3};

	m_vertices = new VertexType[m_vertexCount];
	if (!m_vertices)
	{
		return false;
	}
	memset(m_vertices, 0, (sizeof(VertexType) * m_vertexCount));

	m_vertices[0].texture = VEC2(0.0f, 0.0f);
	m_vertices[1].texture = VEC2(1.0f, 0.0f);
	m_vertices[2].texture = VEC2(0.0f, 1.0f);
	m_vertices[3].texture = VEC2(1.0f, 1.0f);

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = m_vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	HRESULT result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}
	return true;
}

void Bitmap::ReleaseBuffers()
{
	SAFE_RELEASE(m_indexBuffer);
	SAFE_RELEASE(m_vertexBuffer);
}

bool Bitmap::UpdateBuffers(ID3D11DeviceContext *deviceContext, int positionX, int positionY)
{
	if ((positionX == m_previousPosX) && (positionY == m_previousPosY))
	{
		return true;
	}

	m_previousPosX = positionX;
	m_previousPosY = positionY;

	float left = (float)((m_screenWidth / 2) * -1) + (float)positionX;
	float right = left + (float)m_bitmapWidth;
	float top = (float)(m_screenHeight / 2) - (float)positionY;
	float bottom = top - (float)m_bitmapHeight;

	m_vertices[0].position = VEC3(left, top, 0.0f);		// Top left.
	m_vertices[1].position = VEC3(right, top, 0.0f);	// Top right.
	m_vertices[2].position = VEC3(left, bottom, 0.0f);  // Bottom left.
	m_vertices[3].position = VEC3(right, bottom, 0.0f); // Bottom right.

	// Lock the vertex buffer so it can be written to.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	VertexType *verticesPtr = (VertexType *)mappedResource.pData;
	memcpy(verticesPtr, (void *)m_vertices, (sizeof(VertexType) * m_vertexCount));
	deviceContext->Unmap(m_vertexBuffer, 0);

	return true;
}

void Bitmap::RenderBuffers(ID3D11DeviceContext *deviceContext)
{

	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool Bitmap::LoadTexture(ID3D11Device *device, const WCHAR *filename)
{
	m_texture = new Texture;
	if (!m_texture)
	{
		return false;
	}
	bool result = m_texture->Initialize(device, filename);
	if (!result)
	{
		return false;
	}
	return true;
}

void Bitmap::ReleaseTexture()
{
	SAFE_RELEASE_DELETE(m_texture);
}