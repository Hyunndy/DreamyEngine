#pragma once

#include "ShaderClass.h"

/*
2개의 텍스처배열을 사용한다.
*/

class SpecMapShaderClass :public ShaderClass
{
private:


	struct CameraBufferType
	{
		D3DXVECTOR3 cameraPosition;
		float padding;
	};

	struct LightBufferType
	{
		D3DXVECTOR4 ambientColor;
		D3DXVECTOR4 diffuseColor;
		D3DXVECTOR4 specularColor;
		float specularPower;
		D3DXVECTOR3 lightDirection;

	};

public:

	SpecMapShaderClass();
	SpecMapShaderClass(const SpecMapShaderClass&);
	~SpecMapShaderClass();


	virtual bool Initialize( HWND);
	bool Render( int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView**, D3DXVECTOR3,D3DXVECTOR4,  D3DXVECTOR4, D3DXVECTOR3, D3DXVECTOR4, float);

private:

	virtual bool InitializeShader( HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	bool SetShaderParameters(D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView**, D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR4, D3DXVECTOR3, D3DXVECTOR4, float);


private:

	ID3D11Buffer* m_lightBuffer = nullptr; // 조명 상수 버퍼
	ID3D11Buffer* m_cameraBuffer = nullptr; // 카메라 상수 버퍼

};

