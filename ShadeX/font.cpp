#include "font.h"

Font::Font()
{
	m_texture = NULL;
}

Font::Font(const Font &other)
{
	UNREFERENCED_PARAMETER(other);
}

Font::~Font()
{
}

bool Font::Initialize(ID3D11Device *device, const WCHAR *textureFile, const WCHAR *dataFile)
{
	bool result = LoadFontData(dataFile);
	if (!result)
	{
		return false;
	}
	result = LoadTexture(device, textureFile);
	if (!result)
	{
		return false;
	}
	return true;
}

bool Font::LoadTexture(ID3D11Device *device, const WCHAR *fileName)
{
	m_texture = new Texture;
	if (!m_texture)
	{
		return false;
	}
	bool result = m_texture->Initialize(device, fileName);
	if (!result)
	{
		return false;
	}
	return true;
}

bool Font::LoadFontData(const WCHAR *fileName)
{
	ifstream fileStream(fileName);

	if (fileStream.fail())
	{
		return false;
	}

	string line, read, key, data;
	size_t i;

	int lineHeight = 0;
	int base = 0;
	int width = 0;
	int height = 0;
	int pages = 0;
	int outline = 0;

	while (!fileStream.eof())
	{
		stringstream lineStream;

		getline(fileStream, line);

		lineStream << line;
		lineStream >> read;

		if (read == "common")
		{
			while (!lineStream.eof())
			{
				stringstream converter;
				lineStream >> read;
				i = read.find('=');
				key = read.substr(0, i);
				data = read.substr(i + 1);
				converter << data;

				if (key == "lineHeight")
				{
					converter >> lineHeight;
				}
				else if (key == "base")
				{
					converter >> base;
				}
				else if (key == "scaleW")
				{
					converter >> width;
				}
				else if (key == "scaleH")
				{
					converter >> height;
				}
				else if (key == "pages")
				{
					converter >> pages;
				}
				else if (key == "outline")
				{
					converter >> outline;
				}
			}
		}
		else if (read == "char")
		{
			int charID = 0;
			FontType type;

			while (!lineStream.eof())
			{
				stringstream converter;
				lineStream >> read;
				i = read.find('=');
				key = read.substr(0, i);
				data = read.substr(i + 1);

				converter << data;
				if (key == "id")
				{
					converter >> charID;
					type.ch = (char)charID;
				}
				else if (key == "x")
				{
					converter >> type.left;
					type.left /= (float)width;
				}
				else if (key == "y")
				{
					converter >> type.top;
					type.top /= (float)height;
				}
				else if (key == "width")
				{
					converter >> type.width;
					type.right = type.left + type.width / (float)width;
				}
				else if (key == "height")
				{
					converter >> type.height;
					type.bottom = type.top + type.height / (float)height;
				}
				else if (key == "xoffset")
				{
					converter >> type.xOffset;
				}
				else if (key == "yoffset")
				{
					converter >> type.yOffset;
				}
				else if (key == "xadvance")
				{
					converter >> type.xAdvance;
				}
			}
			m_fontMap.insert(make_pair(charID, type));
		}
	}

	fileStream.close();
	return true;
}

void Font::Release()
{
	ReleaseFontData();
	ReleaseTexture();
}

void Font::BuildVertexArray(const char *sentence, float scale, float positionX, float positionY, PACKEDVECTOR &outPacked)
{
	int numLetters = (int)(strlen(sentence));
	int letter;

	float x = positionX;
	float y = positionY;

	float top, bottom, left, right;
	float width, height, xOffset, yOffset;

	UINT index = 0;

	for (int i = 0; i < numLetters; i++)
	{
		letter = (int)sentence[i];

		if (letter == 32)
		{
			x = x + m_fontMap[letter].xAdvance / 2 * scale;
		}
		else
		{
			top = m_fontMap[letter].top;
			bottom = m_fontMap[letter].bottom;
			left = m_fontMap[letter].left;
			right = m_fontMap[letter].right;
			width = m_fontMap[letter].width * scale;
			height = m_fontMap[letter].height * scale;

			xOffset = x;
			yOffset = y - m_fontMap[letter].yOffset * scale;

			x = x + width + 1.05f;

			outPacked[index + 0].position = VEC3(xOffset, yOffset, 0.0f);
			outPacked[index + 0].texture = VEC2(left, top);

			outPacked[index + 1].position = VEC3(xOffset + width, yOffset, 0.0f);
			outPacked[index + 1].texture = VEC2(right, top);

			outPacked[index + 2].position = VEC3(xOffset, yOffset - height, 0.0f);
			outPacked[index + 2].texture = VEC2(left, bottom);

			outPacked[index + 3].position = VEC3(xOffset + width, yOffset - height, 0.0f);
			outPacked[index + 3].texture = VEC2(right, bottom);

			index += 4;
		}
	}
}

void Font::ReleaseFontData()
{
	m_fontMap.clear();
}

void Font::ReleaseTexture()
{
	SAFE_RELEASE_DELETE(m_texture);
}

ID3D11ShaderResourceView *Font::GetTexture() const
{
	return m_texture->GetTexture();
}
