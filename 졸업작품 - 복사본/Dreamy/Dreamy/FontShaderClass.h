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

	// font.ps�� ���� ������ ����ü�� �����Ѵ�.
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

	// ������ ������ ������ ��� ���۸� �����.
	ID3D11Buffer* m_pixelBuffer;
};

