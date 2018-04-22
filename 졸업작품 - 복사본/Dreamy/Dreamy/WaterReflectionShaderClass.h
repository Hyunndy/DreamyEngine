#pragma once
#include "ShaderClass.h"


/*
�̸�: WaterReflectionShaderClass
�뵵:
- ȣ���� �������ϱ����� ReflectionShader
- TerrainShader�� Clip��鸸 �ִ´�.
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
		// D3D11_BIND_CONSTANT_BUFFER�� ����ϸ� BythWidth�� �׻� 16�� ��� ���� �ϸ� �׷��� ������ �����Ѵ�.
	};

	struct ClipPlaneBufferType
	{
		D3DXVECTOR4 clipPlane;
	};

public:
	WaterReflectionShaderClass();
	WaterReflectionShaderClass(const WaterReflectionShaderClass&);
	~WaterReflectionShaderClass();

	virtual bool Initialize( HWND);

	bool Render( int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, D3DXVECTOR4,
		D3DXVECTOR3, float, D3DXVECTOR4);


	bool SetShaderParameters( D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, D3DXVECTOR4,
		D3DXVECTOR3, float, D3DXVECTOR4);

private:

	virtual bool InitializeShader( HWND, WCHAR*, WCHAR*);
	virtual void ShutdownShader();


private:

	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_clipPlaneBuffer;


};

