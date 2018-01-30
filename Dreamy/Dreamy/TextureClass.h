////////////////////////////////////////////////////////////////////////////////
// Filename: textureclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TEXTURECLASS_H_
#define _TEXTURECLASS_H_


//////////////
// INCLUDES //
//////////////
#include "stdafx.h"

/*
	Texture클래스는 텍스처 자원을 불러오고, 해제하고, 접근하는 작업을 캡슐화 한다.
	모든 텍스처에 대해 각각 이 클래스가 만들어져야 한다.

	18.01.26 멀티텍스쳐가 가능하게 한다.
	18.01.27 알파맵텍스처가 가능하다.
*/

////////////////////////////////////////////////////////////////////////////////
// Class name: TextureClass
////////////////////////////////////////////////////////////////////////////////
class TextureClass
{
public:
	TextureClass();
	TextureClass(const TextureClass&);
	~TextureClass();

	// 주어진 파일 이름으로 텍스처를 불러오고 더이상 사용되지 않는 텍스처를 해제한다.
	bool Initialize(ID3D11Device*, WCHAR*);
	bool InitializeMulti(ID3D11Device*, WCHAR*, WCHAR*);
	bool InitializeTriple(ID3D11Device*, WCHAR*, WCHAR*, WCHAR*);


	void Shutdown();

	// 셰이더가 그리는 작업에 사용할 수 있도록 텍스처 자원의 포인터를 반환한다.
	ID3D11ShaderResourceView* GetTexture();
	ID3D11ShaderResourceView** GetMultiTextureArray();
	ID3D11ShaderResourceView** GetTripleTextureArray();

private:

	// 텍스처 자원!
	// ShaderResourceView인걸 기억하자! 텍스처자원은 렌더링 파이프라인에 리소스 뷰 형태로 묶인다!
	ID3D11ShaderResourceView* m_texture;

	ID3D11ShaderResourceView* m_multitexture[2];

	ID3D11ShaderResourceView* m_tripletexture[3];
};

#endif