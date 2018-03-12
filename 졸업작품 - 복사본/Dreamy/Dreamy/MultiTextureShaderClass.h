#pragma once

#include "ShaderClass.h"
/*
	2개의 텍스처배열을 사용한다.
*/

class MultiTextureShaderClass : public ShaderClass
{
private: 


public:

	MultiTextureShaderClass();
	MultiTextureShaderClass(const MultiTextureShaderClass&);
	~MultiTextureShaderClass(); 
	
	
	virtual bool Initialize(ID3D11Device*, HWND);
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView**);

private:

	virtual bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	virtual void ShutdownShader();

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView**);


private:

};


