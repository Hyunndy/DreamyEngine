////////////////////////////////////////////////////////////////////////////////
// Filename: TransparentShaderClass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ShaderClass.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: TransparentShaderClass
////////////////////////////////////////////////////////////////////////////////
class TransparentShaderClass : public ShaderClass
{
private:

	//투명도 버퍼
	struct TransparentBufferType
	{
		float blendAmount;
		D3DXVECTOR3 padding;
	};

public:
	TransparentShaderClass();
	TransparentShaderClass(const TransparentShaderClass&);
	~TransparentShaderClass();


	virtual bool Initialize(ID3D11Device*, HWND);
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, float);

private:
	virtual bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	virtual void ShutdownShader();

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, float);

private:

	ID3D11Buffer* m_transparentBuffer;

};


