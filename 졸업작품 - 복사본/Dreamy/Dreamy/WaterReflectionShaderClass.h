#pragma once
#include "ShaderClass.h"


/*
이름: WaterReflectionShaderClass
용도:
- 호수를 렌더링하기위한 ReflectionShader
- TerrainShader에 Clip평면만 넣는다.
*/


class WaterReflectionShaderClass :
	public ShaderClass
{
private:

	struct LightBufferType
	{
		D3DXVECTOR4 lightDiffuseColor;
		D3DXVECTOR3 lightDirection;
		float colorTextureBrightness;
		// D3D11_BIND_CONSTANT_BUFFER을 사용하면 BythWidth가 항상 16의 배수 여야 하며 그렇지 않으면 실패한다.
	};

	struct ClipPlaneBufferType
	{
		D3DXVECTOR4 clipPlane;
	};

public:
	WaterReflectionShaderClass();
	WaterReflectionShaderClass(const WaterReflectionShaderClass&);
	~WaterReflectionShaderClass();

	virtual bool Initialize(ID3D11Device*, HWND);

	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, D3DXVECTOR4,
		D3DXVECTOR3, float, D3DXVECTOR4);


	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, D3DXVECTOR4,
		D3DXVECTOR3, float, D3DXVECTOR4);

private:

	virtual bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	virtual void ShutdownShader();


private:

	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_clipPlaneBuffer;


};

