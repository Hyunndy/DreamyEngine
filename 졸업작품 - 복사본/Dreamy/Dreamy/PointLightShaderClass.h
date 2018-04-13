////////////////////////////////////////////////////////////////////////////////
// Filename: PointLightShaderClass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ShaderClass.h"

/*
�̸�: PointLightShaderClass
�뵵: �� ������ �ٷ�� ���̴� Ŭ����.
*/

////////////////////////////////////�ٷ�� ������ ����/////////////////////////////////////////////////
const int NUM_LIGHTS = 1;

class PointLightShaderClass : public ShaderClass
{
private:


	struct LightColorBufferType
	{
		D3DXVECTOR4 diffuseColor[NUM_LIGHTS];
	};

	struct LightPositionBufferType
	{
		D3DXVECTOR4 lightPosition[NUM_LIGHTS];
	};
	

public:
	PointLightShaderClass();
	PointLightShaderClass(const PointLightShaderClass&);
	~PointLightShaderClass();

	virtual bool Initialize(ID3D11Device*, HWND);
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, D3DXVECTOR4[],D3DXVECTOR4[]);

private:

	virtual bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	virtual void ShutdownShader();
	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, D3DXVECTOR4[],
		D3DXVECTOR4[]);


private:

	ID3D11Buffer* m_lightColorBuffer;
	ID3D11Buffer* m_lightPositionBuffer;

};

