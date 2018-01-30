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
	TextureŬ������ �ؽ�ó �ڿ��� �ҷ�����, �����ϰ�, �����ϴ� �۾��� ĸ��ȭ �Ѵ�.
	��� �ؽ�ó�� ���� ���� �� Ŭ������ ��������� �Ѵ�.

	18.01.26 ��Ƽ�ؽ��İ� �����ϰ� �Ѵ�.
	18.01.27 ���ĸ��ؽ�ó�� �����ϴ�.
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

	// �־��� ���� �̸����� �ؽ�ó�� �ҷ����� ���̻� ������ �ʴ� �ؽ�ó�� �����Ѵ�.
	bool Initialize(ID3D11Device*, WCHAR*);
	bool InitializeMulti(ID3D11Device*, WCHAR*, WCHAR*);
	bool InitializeTriple(ID3D11Device*, WCHAR*, WCHAR*, WCHAR*);


	void Shutdown();

	// ���̴��� �׸��� �۾��� ����� �� �ֵ��� �ؽ�ó �ڿ��� �����͸� ��ȯ�Ѵ�.
	ID3D11ShaderResourceView* GetTexture();
	ID3D11ShaderResourceView** GetMultiTextureArray();
	ID3D11ShaderResourceView** GetTripleTextureArray();

private:

	// �ؽ�ó �ڿ�!
	// ShaderResourceView�ΰ� �������! �ؽ�ó�ڿ��� ������ ���������ο� ���ҽ� �� ���·� ���δ�!
	ID3D11ShaderResourceView* m_texture;

	ID3D11ShaderResourceView* m_multitexture[2];

	ID3D11ShaderResourceView* m_tripletexture[3];
};

#endif