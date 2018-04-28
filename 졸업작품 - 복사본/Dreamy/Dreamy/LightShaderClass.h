////////////////////////////////////////////////////////////////////////////////
// Filename: LightShaderClass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ShaderClass.h"

/*
	���̴�: ���� ���� �������� �����ϴ� ���� ���α׷�
	HLSL: ���� ���̴�, �ȼ� ���̴��� �ۼ��ϴµ� ���Ǵ� ���

	LightShaderClass�� GPU�� �����ϴ� 3D �𵨵��� �׸��µ� ����ϴ� HLSL���̴�(����, ����, �ȼ�)�� ȣ���ϴ� Ŭ�����̴�.
	HLSL������ �ؽ�Ʈ ���Ϸ� �����Ѵ�.
	���⼱ �װ� �θ��°���!

	18.01.18
	�÷�+�ؽ�ó ���̴�

	18.01.18
	�÷�+�ؽ���+���Ɽ(directional) + ���ݻ籤(specular) ���̴�
	
*/
////////////////////////////////////////////////////////////////////////////////
// Class name: LightShaderClass
////////////////////////////////////////////////////////////////////////////////
class LightShaderClass : public ShaderClass
{
private:

	
	// ���� ���̴� ���Ͽ� �°� ī�޶� ���� ����ü�� �����Ѵ�.
	// createbuffer�Լ��� �������� �ʵ��� padding�� �ٿ� sizeof���� �� 16�� ����� �ǵ����Ѵ�.
	struct CameraBufferType
	{
		D3DXVECTOR3 cameraPosition;
		float padding; // ����ü�� ũ�Ⱑ 16����� �ǰ� �ϱ� ���� �Ⱦ��� ���� �߰�
					   // �̰� ������ CreateBuffer�� ������ 16����� ũ�⸦ �䱸�ϱ� ������ �����Ѵ�.
	};
	// ���� ������ �����ϱ� ���� ����
	// �ȼ����̴��� ����ü ����� �����ؾ� �Ѵ�.
	//LightBufferType ����ü�� �ȼ� ���̴��� ��� ���ۿ� ���������� �ݻ籤�� ����� ������ �����ϵ��� �ٲ�����ϴ�.
	//�����ؾ� �� ���� ������ 16����Ʈ�� ��� ũ�⸦ �����ϱ� ���� padding�� ���ְ� �� �ڸ��� specularPower�� �־��ٴ� ����Դϴ�. 
	//���� padding�� ���� �ʰ� lightDirection �ٷ� �ؿ� specularColor�� �־��ٸ� ���̴��� �ùٷ� �������� �ʾ��� ���Դϴ�.
	//����ü�� ũ�Ⱑ 16byte�� ����̱� ������ �� �������� 16byte�� ���ĵ��� �ʾұ� �����Դϴ�
	struct LightBufferType
	{
		D3DXVECTOR4 ambientColor;
		D3DXVECTOR4 diffuseColor; 
		D3DXVECTOR3 lightDirection;
		float specularPower;
		D3DXVECTOR4 specularColor;
	};
	
public:
	LightShaderClass();
	LightShaderClass(const LightShaderClass&);
	~LightShaderClass();

	virtual bool Initialize( HWND);
	bool Render( int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR4, D3DXVECTOR3, D3DXVECTOR4, float);

private:

	virtual bool InitializeShader(HWND, WCHAR*, WCHAR*);
	virtual void ShutdownShader();
	bool SetShaderParameters( D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR4, D3DXVECTOR3, D3DXVECTOR4, float );


private:


	ID3D11Buffer* m_lightBuffer  = nullptr; // ���Ɽ�� ����(����� ����)�� ���� ��� ����
								// �� ������۴� HLSL�ȼ� ���̴����� ���� ���� �������� �����ϱ� ���� ���ȴ�.

	ID3D11Buffer* m_cameraBuffer = nullptr; // ���ݻ籤�� ���� ���� ���̴����� ī�޶��� ��ġ�� ��� �� ����� ī�޶� ��� ����

};

