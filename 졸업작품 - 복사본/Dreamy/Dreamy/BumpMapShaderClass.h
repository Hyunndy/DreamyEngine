#pragma once
#ifndef _BUMPMAPSHADERCLASS_H_
#define _BUMPMAPSHADERCLASS_H_


#include "ShaderClass.h"


/*
2���� �ؽ�ó�迭�� ����Ѵ�.
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

	ID3D11Buffer* m_lightBuffer; // ���� ����� ������ ������ ��� ����(�ȼ����̴��� ����)

};

#endif
