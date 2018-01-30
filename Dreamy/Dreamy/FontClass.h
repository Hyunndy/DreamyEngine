#ifndef _FONTCLASS_H_
#define _FONTCLASS_H_

#pragma once
#include "stdafx.h"
#include <fstream>
#include "TextureClass.h"

/*--------------------------------------------------------------------------------------------------------
�̸� : FontClass
�뵵 :
- text�� ����ϱ� ���ؼ� �۲� �̹���(�ؽ�ó), �� ���ڰ� �ؽ�ó�� ��� �κп� �ִ��� �˷��ִ� �ؽ�Ʈ ����

- �� Ŭ������ �۲� �ؽ�ó, �ؽ�Ʈ ���Ϸ� ���� �۲� �����͸� �о���δ�.
- �� �����͸� �̿��� �������۸� �����Ѵ�.
- ���⼭ ������ ���� �迭�� TextClass���� ���� ���ۿ� �ִ´�.

- ������ �۲� �����͸� ���� ���� ���۴� TextClass���� �����Ѵ�.(graphicsClass���� ����ϴ°� TextClass)

- DX11���� 2���� �ﰢ������ �̷���� �簢���� ����� �� �簢���� ���ϴ� ������ �ؽ�ó�� �׸���.
- ���ϴ� ������ �ؽ�ó���� ����ϱ� ���ؼ� �ؽ�ó�� �ε����� ���̴°� �ʿ��ѵ�, �� �� �ؽ�Ʈ ������ �ʿ��ϴ�.
- �绡�� �׸� �� �ִ� �ȼ� ��ġ�� ��´�.
----------------------------------------------------------------------------------------------------------*/

class FontClass
{
	// FontType����ü�� �ؽ�ó���Ͽ��� ���� �ϳ��� ����ϱ� ���� �۲� �ε��� ������ �ε��� �����͸� �����ϱ� ���� ���ȴ�.
	struct FontType
	{
		float left, right; //�ؽ�ó�� U��ǥ
		int size; //������ �ȼ� �ʺ�
	};

	// VertexType ����ü�� ������ ���ڰ� �׷��� �簢���� ����� �� �ʿ��� ���� �����͸� �����Ѵ�.
	struct VertexType
	{ 
		// �ؽ�ó�� ����� �Ǵϱ� �������� ��ġ, �ؽ�ó ��ġ���� ������.
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

	// ���ڿ��� �Է����� �޾� ���ڸ� �׸� �ﰢ������ ���� ���۸� ����� ��ȯ�Ѵ�.
	// �׷��� ������ ���� �� TextClass���� ȣ��ȴ�.
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

