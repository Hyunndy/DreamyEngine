#pragma once
#include "ShaderClass.h"
class DiffuseInstancingShaderClass :
	public ShaderClass
{
	struct LightBufferType
	{
		D3DXVECTOR4 diffuseColor;
		D3DXVECTOR3 lightDirection;
		float padding;  // Added extra padding so structure is a multiple of 16 for CreateBuffer function requirements.
	};
public:
	DiffuseInstancingShaderClass();
	DiffuseInstancingShaderClass(const DiffuseInstancingShaderClass&);
	~DiffuseInstancingShaderClass();

	virtual bool Initialize(HWND);
	bool Render(int, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, D3DXVECTOR3, D3DXVECTOR4);

private:
	virtual bool InitializeShader(HWND, WCHAR*, WCHAR*);
	virtual void ShutdownShader();

	bool SetShaderParameters(D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, D3DXVECTOR3, D3DXVECTOR4);
	virtual void RenderShader(int, int);

private:
	ID3D11Buffer* m_lightBuffer = nullptr;
};

