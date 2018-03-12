#pragma once
#ifndef _BUMPMAPSHADERCLASS_H_
#define _BUMPMAPSHADERCLASS_H_


#include "ShaderClass.h"


/*
2개의 텍스처배열을 사용한다.
*/

class BumpMapShaderClass : public ShaderClass
{
private:


	struct LightBufferType
	{
		D3DXVECTOR4 diffuseColor;
		D3DXVECTOR3 lightDirection;
		float padding;
	};

public:

	BumpMapShaderClass();
	BumpMapShaderClass(const BumpMapShaderClass&);
	~BumpMapShaderClass();


	virtual bool Initialize(ID3D11Device*, HWND);

	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView**, D3DXVECTOR3, D3DXVECTOR4);

private:

	virtual bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	virtual void ShutdownShader();

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView**, D3DXVECTOR3, D3DXVECTOR4);


private:

	ID3D11Buffer* m_lightBuffer; // 빛의 방향과 색상을 저장할 상수 버퍼(픽셀셰이더에 있음)

};

#endif
