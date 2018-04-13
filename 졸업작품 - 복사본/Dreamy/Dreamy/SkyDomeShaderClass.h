#pragma once
#include "ShaderClass.h"




class SkyDomeShaderClass : public ShaderClass
{
private:
	struct GradientBufferType
	{
		D3DXVECTOR4 apexColor;
		D3DXVECTOR4 centerColor;
	};


public:
	SkyDomeShaderClass();
	SkyDomeShaderClass(const SkyDomeShaderClass&);
	~SkyDomeShaderClass();
	
	virtual bool Initialize( HWND);
	bool Render(int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXVECTOR4, D3DXVECTOR4);

private:
	virtual bool InitializeShader( HWND, WCHAR*, WCHAR*);
	bool SetShaderParameters( D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXVECTOR4, D3DXVECTOR4);
	
	virtual void ShutdownShader();

private:
	ID3D11Buffer* m_gradientBuffer;

};

