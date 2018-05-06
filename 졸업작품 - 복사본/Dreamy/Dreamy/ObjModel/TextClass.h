////////////////////////////////////////////////////////////////////////////////
// Filename: textclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TEXTCLASS_H_
#define _TEXTCLASS_H_

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "FontClass.h"
#include "../FontShaderClass.h"


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

	bool Initialize(HWND, int, int, D3DXMATRIX);
	void Shutdown();
	void EndShutdown();
	bool Render( D3DXMATRIX, D3DXMATRIX);
	void EndRender(D3DXMATRIX, D3DXMATRIX);

	//GraphicsClass���� ���콺,fps,cpu�� ������ ���� �� Text�� ����Ϸ��� ���� �Լ���.
	bool SetMousePosition(int, int);
	bool SetPosition(float, float, float);
	bool SetFps(int); 
	bool SetTime(int);
	bool SetEndTime(int);



private:
	bool InitializeSentence(SentenceType**, int);
	bool UpdateSentence(SentenceType*, char*, int, int, float, float, float);
	void ReleaseSentence(SentenceType**);
	bool RenderSentence( SentenceType*, D3DXMATRIX, D3DXMATRIX);



private:
	FontClass* m_Font = nullptr;
	FontShaderClass* m_FontShader = nullptr;

	int m_screenWidth, m_screenHeight;

	D3DXMATRIX m_baseViewMatrix;

	SentenceType* m_sentence1= nullptr; // Title���

	SentenceType* m_sentence2=nullptr; // FPS���
	SentenceType* m_sentence3=nullptr; // Time���
						
	SentenceType* m_sentence4=nullptr; // PosX
	SentenceType* m_sentence5=nullptr; // PosY
	SentenceType* m_sentence6=nullptr; // PosZ
							
	SentenceType* m_sentence7=nullptr; // mouseposx
	SentenceType* m_sentence8=nullptr; // mouseposy

	SentenceType* m_sentence9 = nullptr; // ���� �� �� �ɷȴ��� ���

	
};

#endif