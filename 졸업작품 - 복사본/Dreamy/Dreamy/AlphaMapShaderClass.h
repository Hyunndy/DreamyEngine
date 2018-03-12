#pragma once

#include "ShaderClass.h"


/*
2���� �ؽ�ó�迭�� ����Ѵ�.
*/

class AlphaMapShaderClass : public ShaderClass
{
private:

public:

	AlphaMapShaderClass();
	AlphaMapShaderClass(const AlphaMapShaderClass&);
	~AlphaMapShaderClass();


	virtual bool Initialize(ID3D11Device*, HWND);
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView**);

private:

	virtual bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	
	void ShutdownShader();
	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView**);

};


