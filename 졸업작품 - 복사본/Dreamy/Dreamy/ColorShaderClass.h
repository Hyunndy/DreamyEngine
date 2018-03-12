#pragma once
#include "ShaderClass.h"


class ColorShaderClass :
	public ShaderClass
{
public:
	ColorShaderClass();
	ColorShaderClass(const ColorShaderClass&);
	~ColorShaderClass();

	virtual bool Initialize(ID3D11Device*,  HWND);
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX);


private:

	virtual bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);

	void ShutdownShader();
	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX);
};

