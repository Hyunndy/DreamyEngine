////////////////////////////////////////////////////////////////////////////////
// Filename: textureclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "textureclass.h"


TextureClass::TextureClass()
{
	m_multitexture[0] = 0;
	m_multitexture[1] = 0;
	
	m_tripletexture[0] = 0;
	m_tripletexture[1] = 0;
	m_tripletexture[2] = 0;
}


TextureClass::TextureClass(const TextureClass& other)
{
}


TextureClass::~TextureClass()
{
}

/*
	Direct3D ����̽��� �ؽ�ó�� ���� �̸��� ������ m_texture�̶�� ���̴� �ڿ� ������ �ؽ�ó ������ �ε��Ѵ�.
	���⼭ �ؽ�ó�ڿ��� m_texture�� resourceview�ΰ��� �������!
*/
bool TextureClass::Initialize(ID3D11Device* device, WCHAR* filename)
{
	HRESULT result;


	// �̹��� ���Ϸκ��� d3d11texture2d ��ü ���� + shaderresourceview�� �������ִ� �Լ�!
	result = D3DX11CreateShaderResourceViewFromFile(device, filename, NULL, NULL, &m_texture, NULL);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool TextureClass::InitializeMulti(ID3D11Device* device, WCHAR* filename, WCHAR* filename2)
{
	HRESULT result;


	// �̹��� ���Ϸκ��� d3d11texture2d ��ü ���� + shaderresourceview�� �������ִ� �Լ�!
	result = D3DX11CreateShaderResourceViewFromFile(device, filename, NULL, NULL, &m_multitexture[0], NULL);
	if (FAILED(result)){	return false;	}

	result = D3DX11CreateShaderResourceViewFromFile(device, filename2, NULL, NULL, &m_multitexture[1], NULL);
	if (FAILED(result)) { return false; }

	return true;
}

bool TextureClass::InitializeTriple(ID3D11Device* device, WCHAR* filename, WCHAR* filename2, WCHAR* filename3)
{
	HRESULT result;


	// �̹��� ���Ϸκ��� d3d11texture2d ��ü ���� + shaderresourceview�� �������ִ� �Լ�!
	result = D3DX11CreateShaderResourceViewFromFile(device, filename, NULL, NULL, &m_tripletexture[0], NULL);
	if (FAILED(result)) { return false; }

	result = D3DX11CreateShaderResourceViewFromFile(device, filename2, NULL, NULL, &m_tripletexture[1], NULL);
	if (FAILED(result)) { return false; }

	result = D3DX11CreateShaderResourceViewFromFile(device, filename3, NULL, NULL, &m_tripletexture[2], NULL);
	if (FAILED(result)) { return false; }

	return true;
}
void TextureClass::Shutdown()
{
	// Release the texture resource.
	if (m_texture) {m_texture->Release();	m_texture = 0;}

	if (m_multitexture[0]) { m_multitexture[0]->Release();	m_multitexture[0] = 0; }
	if (m_multitexture[1]) { m_multitexture[1]->Release();	m_multitexture[1] = 0; }

	if (m_tripletexture[0]) { m_tripletexture[0]->Release(); m_tripletexture[0] = 0; }
	if (m_tripletexture[1]) { m_tripletexture[1]->Release(); m_tripletexture[1] = 0; }
	if (m_tripletexture[2]) { m_tripletexture[2]->Release(); m_tripletexture[2] = 0; }


	return;
}

// �ٸ� ��ü�� �� �ؽ�ó ���̴� �ڿ��� ������ �ʿ䰡 ���� �� ���ȴ�.
// �� �Լ��� ���� �ؽ�ó�� ������ �� �� �ִ�.
ID3D11ShaderResourceView* TextureClass::GetTexture()
{
	return m_texture;
}

ID3D11ShaderResourceView** TextureClass::GetMultiTextureArray()
{
	return m_multitexture;
}

ID3D11ShaderResourceView** TextureClass::GetTripleTextureArray()
{
	return m_tripletexture;
}