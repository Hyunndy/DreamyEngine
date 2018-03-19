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
용도 :
- 어플리케이션이 필요로 하는 모든 2D문자열을 그린다.
- FontClass(글꼴의 기하구조), FontShaderClass(색을 입히고 render한다)을 이용해 2D문자열을 화면에 그린다.
------------------------------------------------------------------------------------------------------------------------------*/

class TextClass
{
private:

	// 각 문장의 렌더링 정보를 저장한다.
	// 각 문장은 버텍스, 인덱스 버퍼를 가지며 이들은 font, fontshader클래스에서 온다.
	struct SentenceType
	{
		ID3D11Buffer *vertexBuffer, *indexBuffer;
		int vertexCount, indexCount, maxLength;
		float red, green, blue;
	};

	// FontClass, Font.vs의 그것과 구조가 일치해야 한다.
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

	//GraphicsClass에서 마우스,fps,cpu를 프레임 마다 얻어서 Text로 출력하려고 만든 함수들.
	bool SetMousePosition(int, int, ID3D11DeviceContext*);
	bool SetPosition(float, float, float, ID3D11DeviceContext*);
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

	SentenceType* m_sentence1; // Title출력

	SentenceType* m_sentence2; // FPS출력
	SentenceType* m_sentence3; // CPU출력

	SentenceType* m_sentence4; // PosX
	SentenceType* m_sentence5; // PosY
	SentenceType* m_sentence6; // PosZ

	SentenceType* m_sentence7; // mouseposx
	SentenceType* m_sentence8; // mouseposy

	
};

#endif