#pragma once

#include "ShaderClass.h"

/*
2개의 텍스처배열을 사용한다.
*/

class TranslateShaderClass :public ShaderClass
{
private:

	struct TranslateBufferType
	{
		float translation;
		D3DXVECTOR3 padding; // 16비트가 되기 위해
	};

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

	TranslateShaderClass();
	TranslateShaderClass(const TranslateShaderClass&);
	~TranslateShaderClass();


	virtual bool Initialize(ID3D11Device*, HWND);
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView**, D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR4, D3DXVECTOR3, D3DXVECTOR4, float, float);

private:

	virtual bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView**, D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR4, D3DXVECTOR3, D3DXVECTOR4, float, float);


private:

	ID3D11Buffer* m_lightBuffer; // 조명 상수 버퍼
	ID3D11Buffer* m_cameraBuffer; // 카메라 상수 버퍼
	ID3D11Buffer* m_translateBuffer; // 텍스처 이동 버퍼 

};

