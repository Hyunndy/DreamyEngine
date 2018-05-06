#pragma once
#include "ShaderClass.h"
class DepthShaderClass :
	public ShaderClass
{
public:
	DepthShaderClass();
	DepthShaderClass(const DepthShaderClass&);
	~DepthShaderClass();

	virtual bool Initialize(HWND);
	bool Render( int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX);

private:
	virtual bool InitializeShader(HWND, WCHAR*, WCHAR*);
	bool SetShaderParameters(D3DXMATRIX, D3DXMATRIX, D3DXMATRIX);
	virtual void ShutdownShader();
};

