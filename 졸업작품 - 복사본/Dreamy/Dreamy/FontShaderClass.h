////////////////////////////////////////////////////////////////////////////////
// Filename: fontshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once


#include "ShaderClass.h"



////////////////////////////////////////////////////////////////////////////////
// Class name: FontShaderClass
////////////////////////////////////////////////////////////////////////////////
class FontShaderClass : public ShaderClass
{
private:

	// font.ps과 같은 구조의 구조체를 선언한다.
	struct PixelBufferType
	{
		D3DXVECTOR4 pixelColor;
	};

public:
	FontShaderClass();
	FontShaderClass(const FontShaderClass&);
	~FontShaderClass();

	virtual bool Initialize( HWND);

	bool Render( int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, D3DXVECTOR4);

private:
	virtual bool InitializeShader( HWND, WCHAR*, WCHAR*);
	void ShutdownShader();

	bool SetShaderParameters( D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, D3DXVECTOR4);


private:

	// 글자의 색상을 저장할 상수 버퍼를 만든다.
	ID3D11Buffer* m_pixelBuffer;
};

