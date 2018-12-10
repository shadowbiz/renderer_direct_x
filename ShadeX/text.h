#ifndef SE_TEXTDX_H
#define SE_TEXTDX_H

#include "globals.h"
#include "alignedbase.h"
#include "font.h"

#define LONGEST_LINE 64
#define MAX_SENTENCES_COUNT 10
#define USEVECTOR 1

SE_ALIGN16 struct Sentence : public seAligned
{
  private:
	typedef Font::VertexType VertexType;

  public:
	Sentence()
	{
		strcpy(text, "empty string");
		worldMatrix = XMMatrixIdentity();
		vertexCount = LONGEST_LINE * 4;
		indexCount = LONGEST_LINE * 6;
		vertices.reserve(indexCount);
		indices.reserve(vertexCount);
		for (UINT i = 0; i < (vertexCount); i++)
		{
			vertices.push_back(VertexType(VEC3(0, 0, 0), VEC2(0, 0)));
		}
		for (DWORD i = 0; i < static_cast<DWORD>(indexCount); i += 4)
		{
			indices.push_back(i + 0);
			indices.push_back(i + 1);
			indices.push_back(i + 2);
			indices.push_back(i + 2);
			indices.push_back(i + 1);
			indices.push_back(i + 3);
		}
	}
	~Sentence()
	{
		vertices.clear();
		indices.clear();
	}

  public:
	char text[LONGEST_LINE];
	PACKEDVECTOR vertices;
	INDEXVECTOR indices;
	MAT4 worldMatrix;

	UINT vertexCount;
	UINT indexCount;

  private:
};

SE_ALIGN16 class Text : public seAligned
{
  public:
	typedef Font::VertexType VertexType;
	typedef vector<Sentence *> SENTENCEVECTOR;

  public:
	Text();
	Text(const Text &other);
	virtual ~Text();

	bool Initialize(D3D *d3D, HWND hwnd, Font *font, float scale = 0.65f);
	void Release();

	bool Update(ID3D11DeviceContext *devCont);
	void Render(ID3D11DeviceContext *devCont);

	int GetIndexCount() const;
	ID3D11ShaderResourceView *GetTexture() const;

	bool UpdateSentence(Sentence &sentence, const char *text, int x, int y);
	Sentence *AddSentence();
	Sentence &GetSentence(UINT i) const;

  private:
	void UpdateVectors();
	bool UpdateVertexArray();
	bool InitializeBuffers(ID3D11Device *);
	void ReleaseBuffers();
	bool UpdateBuffers(ID3D11DeviceContext *);
	void RenderBuffers(ID3D11DeviceContext *);
	void ReleaseSentences();

  private:
	ID3D11Buffer *m_vertexBuffer;
	ID3D11Buffer *m_indexBuffer;

	Font *m_font;

	float m_scale;
	int m_screenWidth;
	int m_screenHeight;

	SENTENCEVECTOR m_sentences;

	UINT m_vertexCount;
	UINT m_indexCount;

	PACKEDVECTOR m_vertices;
	INDEXVECTOR m_indices;

	bool m_hasChanged;
};

#endif // !SE_TEXTDX_H
