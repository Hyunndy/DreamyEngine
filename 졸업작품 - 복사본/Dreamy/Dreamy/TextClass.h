////////////////////////////////////////////////////////////////////////////////
// Filename: textclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TEXTCLASS_H_
#define _TEXTCLASS_H_

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "FontClass.h"
#include "FontShaderClass.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: TextClass
////////////////////////////////////////////////////////////////////////////////
/*----------------------------------------------------------------------------------------------------------------------------
�뵵 :
- ���ø����̼��� �ʿ�� �ϴ� ��� 2D���ڿ��� �׸���.
- FontClass(�۲��� ���ϱ���), FontShaderClass(���� ������ render�Ѵ�)�� �̿��� 2D���ڿ��� ȭ�鿡 �׸���.
------------------------------------------------------------------------------------------------------------------------------*/

class TextClass
{
private:

	// �� ������ ������ ������ �����Ѵ�.
	// �� ������ ���ؽ�, �ε��� ���۸� ������ �̵��� font, fontshaderŬ�������� �´�.
	struct SentenceType
	{
		ID3D11Buffer *vertexBuffer, *indexBuffer;
		int vertexCount, indexCount, maxLength;
		float red, green, blue;
	};

	// FontClass, Font.vs�� �װͰ� ������ ��ġ�ؾ� �Ѵ�.
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};

public:
	TextClass();
	TextClass(const TextClass&);
	~TextClass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, HWND, int, int, D3DXMATRIX);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX);

	//GraphicsClass���� ���콺,fps,cpu�� ������ ���� �� Text�� ����Ϸ��� ���� �Լ���.
	bool SetMousePosition(int, int, ID3D11DeviceContext*);
	bool SetFps(int, ID3D11DeviceContext*); 
	bool SetCpu(int, ID3D11DeviceContext*);


private:
	bool InitializeSentence(SentenceType**, int, ID3D11Device*);
	bool UpdateSentence(SentenceType*, char*, int, int, float, float, float, ID3D11DeviceContext*);
	void ReleaseSentence(SentenceType**);
	bool RenderSentence(ID3D11DeviceContext*, SentenceType*, D3DXMATRIX, D3DXMATRIX);



private:
	FontClass* m_Font;
	FontShaderClass* m_FontShader;

	int m_screenWidth, m_screenHeight;

	D3DXMATRIX m_baseViewMatrix;

	SentenceType* m_sentence1; // Title���

	SentenceType* m_sentence2; // FPS���
	SentenceType* m_sentence3; // CPU���
};

#endif