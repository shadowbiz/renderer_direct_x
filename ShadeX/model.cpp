#include "model.h"

Model::Model()
{
	m_vertexBuffer = NULL;
	m_indexBuffer = NULL;
	m_model = NULL;
	m_indices = NULL;
}

Model::Model(const Model &other)
{
	UNREFERENCED_PARAMETER(other);
}
Model::~Model()
{
}

bool Model::Initialize(ID3D11Device *device, HWND hwnd, ModelType type)
{
	bool result = LoadModel(type);
	if (!result)
	{
		MessageBox(hwnd, L"Could not load model file.", L"Error", MB_OK);
		return false;
	}

	result = InitializeBuffers(device);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize model buffers.", L"Error", MB_OK);
		return false;
	}
	return true;
}

bool Model::Initialize(ID3D11Device *device, HWND hwnd, char *modelFilename)
{

	bool result = LoadModel(modelFilename);
	if (!result)
	{
		MessageBox(hwnd, L"Could not load model file.", L"Error", MB_OK);
		return false;
	}

	result = InitializeBuffers(device);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize model buffers.", L"Error", MB_OK);
		return false;
	}

	return true;
}

void Model::Release()
{
	ReleaseBuffers();
	ReleaseModel();
}

void Model::Render(ID3D11DeviceContext *deviceContext)
{
	RenderBuffers(deviceContext);
}

UINT Model::GetIndexCount() const
{
	return m_indexCount;
}

bool Model::InitializeBuffers(ID3D11Device *device)
{
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = m_model;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	HRESULT hresult = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(hresult))
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
	indexData.pSysMem = m_indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	hresult = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(hresult))
	{
		return false;
	}

	return true;
}

void Model::ReleaseBuffers()
{
	SAFE_RELEASE(m_indexBuffer);
	SAFE_RELEASE(m_vertexBuffer);
}

void Model::ReleaseModel()
{
	SAFE_DELETE_ARRAY(m_model);
	SAFE_DELETE_ARRAY(m_indices);
}

void Model::RenderBuffers(ID3D11DeviceContext *deviceContext)
{
	UINT stride = sizeof(VertexType);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool Model::LoadModel(ModelType type)
{
	m_vertexCount = 4;
	m_indexCount = 6;

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

	const float h = 50 / 2.0f;
	const float w = 50 / 2.0f;

	const float left = -h;
	const float right = h;

	const float front = -w;
	const float back = w;

	m_model[0].texture = VEC2(0, 0);
	m_model[1].texture = VEC2(1, 0);
	m_model[2].texture = VEC2(0, 1);
	m_model[3].texture = VEC2(1, 1);

	m_model[0].position = VEC3(left, 0, back);
	m_model[1].position = VEC3(right, 0, back);
	m_model[2].position = VEC3(left, 0, front);
	m_model[3].position = VEC3(right, 0, front);

	m_model[0].normal = VEC3(0, 1, 0);
	m_model[1].normal = VEC3(0, 1, 0);
	m_model[2].normal = VEC3(0, 1, 0);
	m_model[3].normal = VEC3(0, 1, 0);

	const DWORD planeIndices[] = {0, 1, 2, 2, 1, 3};

	for (UINT i = 0; i < m_indexCount; i++)
	{
		m_indices[i] = planeIndices[i];
	}

	for (UINT i = 0; i < m_vertexCount; i++)
	{
		CalculateTangent(m_model[i].normal, m_model[i].tangent, m_model[i].binormal);
	}

	return true;
}

bool Model::LoadModel(char *fileName)
{
	VEC3VECTOR tempVertices;
	VEC3VECTOR tempNormals;
	VEC2VECTOR tempUvs;

	VEC3VECTOR outVertices;
	VEC3VECTOR outNormals;
	VEC2VECTOR outUvs;

	INDEXVECTOR vertexIndices;
	INDEXVECTOR normalIndices;
	INDEXVECTOR uvIndices;

	FILE *file = fopen(fileName, "r");
	if (file == NULL)
	{
		return false;
	}

	float x = 0;
	float y = 0;
	float z = 0;

	while (true)
	{
		char lineHeader[128];
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
		{
			break;
		}
		if (strcmp(lineHeader, "v") == 0)
		{
			fscanf(file, "%f %f %f\n", &x, &y, &z);
			z *= -1.0f;
			VEC3 vertex = VEC3(x, y, z);
			tempVertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0)
		{
			fscanf(file, "%f %f\n", &x, &y);
			y *= -1.0f;
			VEC2 uv = VEC2(x, y);
			tempUvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0)
		{
			fscanf(file, "%f %f %f\n", &x, &y, &z);
			z *= -1.0f;
			VEC3 normal = VEC3(x, y, z);
			tempNormals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0)
		{
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9)
			{
				return false;
			}
			vertexIndices.push_back(vertexIndex[2]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[0]);
			uvIndices.push_back(uvIndex[2]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[0]);
			normalIndices.push_back(normalIndex[2]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[0]);
		}
		else
		{
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}
	}

	for (DWORD v = 0; v < vertexIndices.size(); v += 3)
	{
		for (DWORD i = 0; i < 3; i += 1)
		{
			DWORD vertexIndex = vertexIndices[v + i];
			VEC3 vertex = tempVertices[vertexIndex - 1];

			DWORD uvIndex = uvIndices[v + i];
			VEC2 uv = tempUvs[uvIndex - 1];

			DWORD normalIndex = normalIndices[v + i];
			VEC3 normal = tempNormals[normalIndex - 1];

			outVertices.push_back(vertex);
			outUvs.push_back(uv);
			outNormals.push_back(normal);
		}
	}

	fclose(file);

	return Optimize(outVertices, outNormals, outUvs);
}

bool Model::GetSimiliarIndex(const VertexType &packed, VERTEXMAP &vertixes, DWORD &index)
{
	map<VertexType, DWORD>::iterator it = vertixes.find(packed);
	if (it == vertixes.end())
	{
		return false;
	}
	else
	{
		index = it->second;
		return true;
	}
}

bool Model::Optimize(const VEC3VECTOR &oldVertices, const VEC3VECTOR &oldNormals, const VEC2VECTOR &oldUvs)
{
	VEC3VECTOR newVertices;
	VEC3VECTOR newNormals;
	VEC2VECTOR newUvs;
	INDEXVECTOR newIndices;

	newVertices.reserve(oldVertices.size());
	newNormals.reserve(oldNormals.size());
	newUvs.reserve(oldUvs.size());
	newIndices.reserve(oldVertices.size());

	VERTEXMAP verticesOutOfIndex;

	DWORD index = 0;
	DWORD newIndex = 0;
	VertexType packed;
	for (DWORD i = 0; i < oldVertices.size(); i++)
	{
		packed = VertexType(oldVertices[i], oldUvs[i], oldNormals[i]);
		index = 0;
		bool found = GetSimiliarIndex(packed, verticesOutOfIndex, index);
		if (found)
		{
			newIndices.push_back(index);
			XMStoreFloat3(&newNormals[index], XMVector3Normalize(XMVectorAdd(XMLoadFloat3(&newNormals[index]), XMLoadFloat3(&oldNormals[i]))));
		}
		else
		{
			newVertices.push_back(oldVertices[i]);
			newNormals.push_back(oldNormals[i]);
			newUvs.push_back(oldUvs[i]);
			newIndex = newVertices.size() - 1;
			newIndices.push_back(newIndex);
			verticesOutOfIndex[packed] = newIndex;
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
	return true;
}

void Model::CalculateTangent(const VEC3 &normal, VEC3 &tangent, VEC3 &binormal)
{
	XMVECTOR n = XMLoadFloat3(&normal);
	XMVECTOR c1 = XMVector3Cross(n, XMVectorSet(0, 0, 1, 0));
	XMVECTOR c2 = XMVector3Cross(n, XMVectorSet(0, 1, 0, 0));

	float l1 = XMVectorGetX(XMVector3Length(c1));
	float l2 = XMVectorGetX(XMVector3Length(c2));

	if (l1 > l2)
	{
		c1 = XMVector3Normalize(c1);
		XMStoreFloat3(&tangent, c1);
		c1 = XMVector3Normalize(XMVector3Cross(n, c1));
		XMStoreFloat3(&binormal, c1);
	}
	else
	{
		c2 = XMVector3Normalize(c2);
		XMStoreFloat3(&tangent, c2);
		c2 = XMVector3Normalize(XMVector3Cross(n, c2));
		XMStoreFloat3(&binormal, c2);
	}
}
