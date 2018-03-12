#pragma once

#include "ShaderClass.h"

/*
2���� �ؽ�ó�迭�� ����Ѵ�.
*/

class TranslateShaderClass :public ShaderClass
{
private:

	struct TranslateBufferType
	{
		float translation;
		D3DXVECTOR3 padding; // 16��Ʈ�� �Ǳ� ����
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

	ID3D11Buffer* m_lightBuffer; // ���� ��� ����
	ID3D11Buffer* m_cameraBuffer; // ī�޶� ��� ����
	ID3D11Buffer* m_translateBuffer; // �ؽ�ó �̵� ���� 

};

