#include "text.h"

Text::Text()
{
	m_vertexBuffer = NULL;
	m_indexBuffer = NULL;
	m_font = NULL;
	m_scale = 1.0f;
	m_hasChanged = true;
}

Text::Text(const Text &other)
{
	UNREFERENCED_PARAMETER(other);
}

Text::~Text()
{
}

void Text::Release()
{
	ReleaseBuffers();
	ReleaseSentences();
}

void Text::ReleaseSentences()
{
	while (!m_sentences.empty())
	{
		Sentence *sentence = m_sentences.back();
		if (sentence)
		{
			sentence->indices.clear();
			sentence->vertices.clear();
			SAFE_DELETE(sentence);
		}
		m_sentences.pop_back();
	}
}

void Text::ReleaseBuffers()
{
	SAFE_RELEASE(m_vertexBuffer);
	SAFE_RELEASE(m_indexBuffer);
}

void Text::Render(ID3D11DeviceContext *devCont)
{
	RenderBuffers(devCont);
}

bool Text::Initialize(D3D *d3D, HWND hwnd, Font *font, float scale)
{

	UNREFERENCED_PARAMETER(hwnd);

	m_font = font;
	m_scale = scale;
	m_screenWidth = d3D->GetScreenWidth();
	m_screenHeight = d3D->GetScreenHeight();
	m_vertexCount = LONGEST_LINE * 4 * MAX_SENTENCES_COUNT;
	m_indexCount = LONGEST_LINE * 6 * MAX_SENTENCES_COUNT;

	m_vertices.reserve(m_vertexCount);
	m_indices.reserve(m_indexCount);

	for (UINT i = 0; i < (m_vertexCount); i++)
	{
		m_vertices.push_back(VertexType(VEC3(0, 0, 0), VEC2(0, 0)));
	}
	for (DWORD i = 0; i < static_cast<DWORD>(m_indexCount); i += 4)
	{
		m_indices.push_back(i + 0);
		m_indices.push_back(i + 1);
		m_indices.push_back(i + 2);
		m_indices.push_back(i + 2);
		m_indices.push_back(i + 1);
		m_indices.push_back(i + 3);
	}

	bool result = InitializeBuffers(d3D->GetDevice());
	if (!result)
	{
		return false;
	}

	return true;
}

Sentence *Text::AddSentence()
{
	Sentence *newSentence = new Sentence;
	if (!newSentence)
	{
		return NULL;
	}
	m_sentences.push_back(newSentence);
	return newSentence;
}

bool Text::UpdateSentence(Sentence &sentence, const char *text, int x, int y)
{
	if (strcmp(sentence.text, text) == 0)
	{
		return true;
	}
	strcpy(sentence.text, text);

	const float drawX = -((float)m_screenWidth / 2.0f) + (float)x;
	const float drawY = (float)m_screenHeight / 2.0f - (float)y;

	m_font->BuildVertexArray(text, m_scale, drawX, drawY, sentence.vertices);

	int numLetters = (int)(strlen(text));

	sentence.indexCount = numLetters * 6;
	sentence.vertexCount = numLetters * 4;

	m_hasChanged = true;

	return true;
}

void Text::UpdateVectors()
{
	m_indexCount = 0;
	m_vertexCount = 0;
	for (Sentence *sentence : m_sentences)
	{
		memcpy(&m_vertices[m_vertexCount], &sentence->vertices[0], sizeof(VertexType) * sentence->vertexCount);
		m_indexCount += sentence->indexCount;
		m_vertexCount += sentence->vertexCount;
	}
}

bool Text::Update(ID3D11DeviceContext *devCont)
{
	if (!m_hasChanged)
	{
		return true;
	}
	UpdateVectors();
	bool result = UpdateBuffers(devCont);
	if (!result)
	{
		return false;
	}
	m_hasChanged = false;
	return true;
}

bool Text::UpdateBuffers(ID3D11DeviceContext *deviceContext)
{
	D3D11_MAPPED_SUBRESOURCE vertexMappedResource;
	HRESULT result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &vertexMappedResource);
	if (FAILED(result))
	{
		return false;
	}
	VertexType *verticesPtr = (VertexType *)vertexMappedResource.pData;
	memcpy(verticesPtr, &m_vertices[0], (sizeof(VertexType) * m_vertexCount));
	deviceContext->Unmap(m_vertexBuffer, 0);
	return true;
}

bool Text::InitializeBuffers(ID3D11Device *device)
{
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = &m_vertices[0];
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

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
	indexData.pSysMem = &m_indices[0];
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}
	return true;
}

void Text::RenderBuffers(ID3D11DeviceContext *deviceContext)
{
	UINT stride = sizeof(VertexType);
	UINT offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

ID3D11ShaderResourceView *Text::GetTexture() const
{
	return m_font->GetTexture();
}

int Text::GetIndexCount() const
{
	return m_indexCount;
}
