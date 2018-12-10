#include "gui.h"

Gui::Gui()
{
	m_D3D = NULL;
	m_shader = NULL;
	m_text = NULL;
	m_font = NULL;
	m_fpsSentence = NULL;
	m_cpuSentence = NULL;
}

Gui::Gui(const Gui &other)
{
	UNREFERENCED_PARAMETER(other);
}

Gui::~Gui()
{
}

bool Gui::Initialize(D3D *d3D, HWND hwnd, TextureShader *shader)
{
	m_D3D = d3D;
	m_shader = shader;

	m_font = new Font;
	if (!m_font)
	{
		return false;
	}
	bool result = m_font->Initialize(m_D3D->GetDevice(), L"data/font_0.dds", L"data/font.fnt");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize font.", L"Error", MB_OK);
		return false;
	}

	m_text = new Text;
	if (!m_text)
	{
		return false;
	}
	result = m_text->Initialize(m_D3D, hwnd, m_font);
	if (!result)
	{
		return false;
	}

	m_fpsSentence = m_text->AddSentence();
	m_cpuSentence = m_text->AddSentence();

	return true;
}

void Gui::Release()
{
	SAFE_RELEASE_DELETE(m_text);
	SAFE_RELEASE_DELETE(m_font);
}

bool Gui::Update(float deltaTime, int fps, int cpuUssage)
{
	char tmpString[36];
	sprintf(tmpString, "FPS: %i", fps);
	bool result = m_text->UpdateSentence(*m_fpsSentence, tmpString, 30, 30);
	if (!result)
	{
		return false;
	}
	sprintf(tmpString, "CPU: %i%%", cpuUssage);
	result = m_text->UpdateSentence(*m_cpuSentence, tmpString, 30, 60);
	if (!result)
	{
		return false;
	}
	result = m_text->Update(m_D3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}
	return true;
}

bool Gui::Render()
{
	m_text->Render(m_D3D->GetDeviceContext());
	const MAT4 &iMat = m_D3D->GetIdentityMatrix();
	const MAT4 &oMat = m_D3D->GetOrthoMatrix();
	bool result = m_shader->Render(m_D3D->GetDeviceContext(), m_text->GetIndexCount(), iMat, iMat, oMat, m_text->GetTexture());
	if (!result)
	{
		return false;
	}
	return true;
}
