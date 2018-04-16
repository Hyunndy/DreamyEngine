#pragma once
#include "ShaderClass.h"



class ParticleShaderClass :
	public ShaderClass
{
public:
	ParticleShaderClass();
	ParticleShaderClass(const ParticleShaderClass&);
	~ParticleShaderClass();

	virtual bool Initialize( HWND);

	bool Render( int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*);

private:



	virtual bool InitializeShader(HWND, WCHAR*, WCHAR*);
	virtual void ShutdownShader();

	bool SetShaderParameters( D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*);
	virtual void RenderShader(int);
};

