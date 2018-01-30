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
	Direct3D 디바이스와 텍스처의 파일 이름을 가지고 m_texture이라는 셰이더 자원 변수에 텍스처 파일을 로드한다.
	여기서 텍스처자원인 m_texture가 resourceview인것을 기억하자!
*/
bool TextureClass::Initialize(ID3D11Device* device, WCHAR* filename)
{
	HRESULT result;


	// 이미지 파일로부터 d3d11texture2d 객체 생성 + shaderresourceview에 연결해주는 함수!
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


	// 이미지 파일로부터 d3d11texture2d 객체 생성 + shaderresourceview에 연결해주는 함수!
	result = D3DX11CreateShaderResourceViewFromFile(device, filename, NULL, NULL, &m_multitexture[0], NULL);
	if (FAILED(result)){	return false;	}

	result = D3DX11CreateShaderResourceViewFromFile(device, filename2, NULL, NULL, &m_multitexture[1], NULL);
	if (FAILED(result)) { return false; }

	return true;
}

bool TextureClass::InitializeTriple(ID3D11Device* device, WCHAR* filename, WCHAR* filename2, WCHAR* filename3)
{
	HRESULT result;


	// 이미지 파일로부터 d3d11texture2d 객체 생성 + shaderresourceview에 연결해주는 함수!
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

// 다른 객체가 이 텍스처 셰이더 자원에 접근할 필요가 있을 때 사용된다.
// 이 함수를 통해 텍스처를 렌더링 할 수 있다.
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