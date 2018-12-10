#include "terrain.h"

Terrain::Terrain() : Model()
{
}

Terrain::Terrain(const Terrain &other)
{
	UNREFERENCED_PARAMETER(other);
}

Terrain::~Terrain()
{
}

bool Terrain::Initialize(ID3D11Device *device, const char *heightMapFilename)
{
	bool result = LoadHeightMap(heightMapFilename);
	if (!result)
	{
		return false;
	}
	NormalizeHeightMap();
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}
	return true;
}

bool Terrain::LoadHeightMap(const char *fileName)
{
	BYTE *bitmapImage;
	bool result = LoadBitmapFromFile(fileName, m_terrainWidth, m_terrainHeight, &bitmapImage);
	if (!result)
	{
		return false;
	}

	float height;
	INT k = 0;
	for (UINT j = 0; j < m_terrainHeight; j++)
	{
		for (UINT i = 0; i < m_terrainWidth; i++)
		{
			height = (float)bitmapImage[k];
			VEC3 position = VEC3((float)i, height / 15.0f, (float)j);
			m_heightMap.push_back(position);
			k += 3;
		}
	}

	SAFE_DELETE_ARRAY(bitmapImage);
	return true;
}

void Terrain::NormalizeHeightMap()
{
}

void Terrain::CalculateFaceNormal(const VEC3 &a, const VEC3 &b, const VEC3 &c, VEC3 &outN) const
{
	XMVECTOR va = XMLoadFloat3(&a);
	XMVECTOR vb = XMLoadFloat3(&b);
	XMVECTOR vc = XMLoadFloat3(&c);
	XMVECTOR vn = XMVector3Normalize(XMVector3Cross(XMVectorSubtract(vb, va), XMVectorSubtract(vc, va)));
	XMStoreFloat3(&outN, vn);
}

void Terrain::CalculateUV(const float x, const float y, const float width, const float height, VEC2 &outUv) const
{
	float uvx = x / width;
	float uvy = y / height;
	outUv = VEC2(uvx, uvy);
}

bool Terrain::InitializeBuffers(ID3D11Device *device)
{
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	m_vertexCount = (m_terrainWidth - 1) * (m_terrainHeight - 1) * 6;
	m_indexCount = m_vertexCount;

	VEC3VECTOR newVertices;
	VEC3VECTOR newNormals;
	VEC2VECTOR newUvs;
	INDEXVECTOR newIndices;

	DWORD index = 0;
	VEC3 a, b, c, d, n1, n2, n;
	VEC2 uvA, uvB, uvC, uvD;
	int indexA, indexB, indexC, indexD;
	for (UINT j = 0; j < (m_terrainHeight - 1); j++)
	{
		for (UINT i = 0; i < (m_terrainWidth - 1); i++)
		{
			indexA = (m_terrainHeight * (j + 1)) + (i + 0); // Upper left.
			indexB = (m_terrainHeight * (j + 1)) + (i + 1); // Upper right.
			indexC = (m_terrainHeight * (j + 0)) + (i + 0); // Bottom left.
			indexD = (m_terrainHeight * (j + 0)) + (i + 1); // Bottom right.

			a = m_heightMap[indexA];
			b = m_heightMap[indexB];
			c = m_heightMap[indexC];
			d = m_heightMap[indexD];

			CalculateFaceNormal(a, b, c, n1);
			CalculateFaceNormal(c, b, d, n2);
			XMStoreFloat3(&n, XMVector3Normalize((XMLoadFloat3(&n1) + XMLoadFloat3(&n2)) / 2));

			CalculateUV((float)(i + 0), (float)(j + 1), (float)(m_terrainWidth - 1), (float)(m_terrainHeight - 1), uvA);
			CalculateUV((float)(i + 1), (float)(j + 1), (float)(m_terrainWidth - 1), (float)(m_terrainHeight - 1), uvB);
			CalculateUV((float)(i + 0), (float)(j + 0), (float)(m_terrainWidth - 1), (float)(m_terrainHeight - 1), uvC);
			CalculateUV((float)(i + 1), (float)(j + 0), (float)(m_terrainWidth - 1), (float)(m_terrainHeight - 1), uvD);

			//////////////////////////////////////////////////////////////////////////

			newVertices.push_back(a);
			newVertices.push_back(b);
			newVertices.push_back(c);
			newNormals.push_back(n);
			newNormals.push_back(n);
			newNormals.push_back(n);
			newUvs.push_back(uvA);
			newUvs.push_back(uvB);
			newUvs.push_back(uvC);
			newIndices.push_back(index++);
			newIndices.push_back(index++);
			newIndices.push_back(index++);

			//////////////////////////////////////////////////////////////////////////

			newVertices.push_back(c);
			newVertices.push_back(b);
			newVertices.push_back(d);
			newNormals.push_back(n);
			newNormals.push_back(n);
			newNormals.push_back(n);
			newUvs.push_back(uvC);
			newUvs.push_back(uvB);
			newUvs.push_back(uvD);
			newIndices.push_back(index++);
			newIndices.push_back(index++);
			newIndices.push_back(index++);
		}
	}

	m_vertexCount = newVertices.size();
	m_indexCount = newIndices.size();

	m_model = new VertexType[m_vertexCount];
	if (!m_model)
	{
		return false;
	}

	m_indices = new DWORD[m_indexCount];
	if (!m_indices)
	{
		return false;
	}

	for (UINT i = 0; i < m_vertexCount; i++)
	{
		m_model[i].position = newVertices[i];
		m_model[i].texture = newUvs[i];
		m_model[i].normal = newNormals[i];
		CalculateTangent(newNormals[i], m_model[i].tangent, m_model[i].binormal);
	}

	memcpy(m_indices, &newIndices[0], sizeof(DWORD) * m_indexCount);

	//Optimize(newVertices, newNormals, newUvs);

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	vertexData.pSysMem = m_model;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = m_indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void Terrain::Render(ID3D11DeviceContext *deviceContext)
{
	RenderBuffers(deviceContext);
}

void Terrain::RenderBuffers(ID3D11DeviceContext *deviceContext)
{
	UINT stride = sizeof(VertexType);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
