#pragma once
#include "ShaderClass.h"
class SkyPlaneShaderClass :
	public ShaderClass
{

private:

	struct SkyBufferType
	{
		float firstTranslationX;
		float firstTranslationZ;
		float secondTranslationX;
		float secondTranslationZ;
		float brightness;
		D3DXVECTOR3 padding;
	};

public:
	SkyPlaneShaderClass();
	SkyPlaneShaderClass(const SkyPlaneShaderClass&);
	~SkyPlaneShaderClass();

	virtual bool Initialize(ID3D11Device*, HWND);
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, float, float, float,
		float, float);

private:
	virtual bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, float, float,
		float, float, float);

	virtual void ShutdownShader();

private:
	ID3D11Buffer* m_skyBuffer;

};

