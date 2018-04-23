#pragma once

#include "ShaderClass.h"


/*
2개의 텍스처배열을 사용한다.
*/

class DiffuseShaderClass : public ShaderClass
{
private:
	struct LightBufferType
	{
		D3DXVECTOR4 diffuseColor;
		D3DXVECTOR3 lightDirection;
		float padding;  // Added extra padding so structure is a multiple of 16 for CreateBuffer function requirements.
	};

public:

	DiffuseShaderClass();
	DiffuseShaderClass(const DiffuseShaderClass&);
	~DiffuseShaderClass();


	virtual bool Initialize( HWND);
	bool Render(int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,
		D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, D3DXVECTOR3 lightDirection, D3DXVECTOR4 diffuseColor);

private:

	virtual bool InitializeShader( HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	bool SetShaderParameters(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,
		D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, D3DXVECTOR3 lightDirection,
		D3DXVECTOR4 diffuseColor);

private:
	ID3D11Buffer* m_lightBuffer;
};


