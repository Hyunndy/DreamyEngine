#pragma once
#include "ShaderClass.h"



class ParticleShaderClass :
	public ShaderClass
{
public:
	ParticleShaderClass();
	ParticleShaderClass(const ParticleShaderClass&);
	~ParticleShaderClass();

	virtual bool Initialize(ID3D11Device*, HWND);

	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*);

private:



	virtual bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	virtual void ShutdownShader();

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*);
	virtual void RenderShader(ID3D11DeviceContext*, int);
};

