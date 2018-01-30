#ifndef _FONTCLASS_H_
#define _FONTCLASS_H_

#pragma once
#include "stdafx.h"
#include <fstream>
#include "TextureClass.h"

/*--------------------------------------------------------------------------------------------------------
이름 : FontClass
용도 :
- text를 출력하기 위해선 글꼴 이미지(텍스처), 각 글자가 텍스처의 어느 부분에 있는지 알려주는 텍스트 파일

- 이 클래스는 글꼴 텍스처, 텍스트 파일로 부터 글꼴 데이터를 읽어들인다.
- 그 데이터를 이용해 정점버퍼를 생성한다.
- 여기서 생겨진 정점 배열을 TextClass에서 정점 버퍼에 넣는다.

- 각각의 글꼴 데이터를 가진 정점 버퍼는 TextClass에서 관리한다.(graphicsClass에서 출력하는게 TextClass)

- DX11에선 2개의 삼각형으로 이루어진 사각형을 만들고 그 사각형에 원하는 글자의 텍스처를 그린다.
- 원하는 글자의 텍스처만을 출력하기 위해선 텍스처에 인덱스를 먹이는게 필요한데, 이 때 텍스트 파일이 필요하다.
- 재빨리 그릴 수 있는 픽셀 위치를 잡는다.
----------------------------------------------------------------------------------------------------------*/

class FontClass
{
	// FontType구조체는 텍스처파일에서 글자 하나를 출력하기 위해 글꼴 인덱스 파일의 인덱스 데이터를 저장하기 위해 사용된다.
	struct FontType
	{
		float left, right; //텍스처의 U좌표
		int size; //문자의 픽셀 너비
	};

	// VertexType 구조체는 실제로 문자가 그려질 사각형을 만드는 데 필요한 정점 데이터를 저장한다.
	struct VertexType
	{ 
		// 텍스처만 씌우면 되니까 공간상의 위치, 텍스처 위치만을 가진다.
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};


public:
	FontClass();
	FontClass(const FontClass&);
	~FontClass();

	bool Initialize(ID3D11Device*, char*, WCHAR*);
	void ShutDown();
	ID3D11ShaderResourceView* GetTexture();

	// 문자열을 입력으로 받아 글자를 그릴 삼각형들의 정점 버퍼를 만들어 반환한다.
	// 그려낼 문장이 있을 때 TextClass에서 호출된다.
	void BuildVertexArray(void*, char*, float, float);

private:
	bool LoadFontPositionTextFile(char*);
	void ReleaseFontData();
	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();


private:
	FontType* m_Font;
	TextureClass* m_Texture;

};
#endif

