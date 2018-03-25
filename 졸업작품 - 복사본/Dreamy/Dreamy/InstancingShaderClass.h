#pragma once
#include "ShaderClass.h"

/*
이름: InstancingShaderClass
용도: 인스턴싱을 위한 TextureShaderClass의 변형 버전.

*/


class InstancingShaderClass : public ShaderClass
{
public:
	InstancingShaderClass();
	InstancingShaderClass(const InstancingShaderClass&);
	~InstancingShaderClass();

	virtual bool Initialize(ID3D11Device*, HWND);
	bool Render(ID3D11DeviceContext*, int, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*);

private:
	virtual bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	virtual void ShutdownShader();

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*);
	virtual void RenderShader(ID3D11DeviceContext*, int, int);



};

