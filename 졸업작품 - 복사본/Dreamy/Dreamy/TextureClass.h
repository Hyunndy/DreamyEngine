////////////////////////////////////////////////////////////////////////////////
// Filename: textureclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TEXTURECLASS_H_
#define _TEXTURECLASS_H_


//////////////
// INCLUDES //
//////////////
#include "stdafx.h"
#include <fstream>

/*
	TextureŬ������ �ؽ�ó �ڿ��� �ҷ�����, �����ϰ�, �����ϴ� �۾��� ĸ��ȭ �Ѵ�.
	��� �ؽ�ó�� ���� ���� �� Ŭ������ ��������� �Ѵ�.

	18.01.26 ��Ƽ�ؽ��İ� �����ϰ� �Ѵ�.
	18.01.27 ���ĸ��ؽ�ó�� �����ϴ�.
	18.03.06 .tga����
*/

////////////////////////////////////////////////////////////////////////////////
// Class name: TextureClass
////////////////////////////////////////////////////////////////////////////////
class TextureClass
{
private:
	struct TargaHeader
	{
		unsigned char data1[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;
	};
public:
	TextureClass();
	TextureClass(const TextureClass&);
	~TextureClass();

	// �־��� ���� �̸����� �ؽ�ó�� �ҷ����� ���̻� ������ �ʴ� �ؽ�ó�� �����Ѵ�.
	bool Initialize(ID3D11Device*, WCHAR*);
	bool InitializeMulti(ID3D11Device*, WCHAR*, WCHAR*);
	bool InitializeTriple(ID3D11Device*, WCHAR*, WCHAR*, WCHAR*);
	bool InitializeTGA(ID3D11Device*, ID3D11DeviceContext*, char*);



	void Shutdown();

	// ���̴��� �׸��� �۾��� ����� �� �ֵ��� �ؽ�ó �ڿ��� �����͸� ��ȯ�Ѵ�.
	ID3D11ShaderResourceView* GetTexture();
	ID3D11ShaderResourceView** GetMultiTextureArray();
	ID3D11ShaderResourceView** GetTripleTextureArray();
	ID3D11ShaderResourceView* GetTGATexture();

private:
	bool LoadTarga(char* filename, int& height, int& width);

private:

	// �ؽ�ó �ڿ�!
	// ShaderResourceView�ΰ� �������! �ؽ�ó�ڿ��� ������ ���������ο� ���ҽ� �� ���·� ���δ�!
	ID3D11ShaderResourceView* m_texture;
	unsigned char* m_targaData;
	ID3D11Texture2D* m_TGAtexture;
	ID3D11ShaderResourceView* m_TGAtextureView;

	ID3D11ShaderResourceView* m_multitexture[2];

	ID3D11ShaderResourceView* m_tripletexture[3];
};

#endif