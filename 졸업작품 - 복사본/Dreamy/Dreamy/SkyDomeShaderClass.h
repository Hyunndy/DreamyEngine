#pragma once
#include "ShaderClass.h"

using namespace std;


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
	
	virtual bool Initialize(ID3D11Device*, HWND);
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXVECTOR4, D3DXVECTOR4);

private:
	virtual bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXVECTOR4, D3DXVECTOR4);
	
	virtual void ShutdownShader();

private:
	ID3D11Buffer* m_gradientBuffer;

};

