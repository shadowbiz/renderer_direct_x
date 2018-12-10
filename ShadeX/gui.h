#ifndef SE_GUIDX_H
#define SE_GUIDX_H

#include "globals.h"
#include "d3d.h"
#include "textureshader.h"
#include "font.h"
#include "text.h"

class Gui
{
  public:
	Gui();
	Gui(const Gui &other);
	~Gui();

	bool Initialize(D3D *d3D, HWND hwnd, TextureShader *shader);
	void Release();

	bool Update(float deltaTime, int, int);
	bool Render();

  private:
	D3D *m_D3D;
	TextureShader *m_shader;
	Font *m_font;
	Text *m_text;
	Sentence *m_fpsSentence;
	Sentence *m_cpuSentence;
};

#endif // !SE_GUIDX_H
