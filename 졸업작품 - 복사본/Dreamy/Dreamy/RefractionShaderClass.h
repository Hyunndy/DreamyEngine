#pragma once
#include "ShaderClass.h"

/*
�̸�: RefractionShaderClass(���� ���̴�)
�뵵:
- ���� ���̴��� ����Ʈ ���̴�(���ݻ籤(=Specular light)�� ����)�� Ŭ���� ����� �߰��� �����̴�.
- ������ ���������� ���� ������ ������ �� �Ʒ��ִ� �͵鸸 �׷����� �ϱ� ������ Ŭ���� ����� ����Ͽ� �ذ��ؾ� �Ѵ�.
*/


class RefractionShaderClass : public ShaderClass
{
private:
	struct LightBufferType
	{
		D3DXVECTOR4 ambientColor;
		D3DXVECTOR4 diffuseColor;
		D3DXVECTOR3 lightDirection;
		float padding;
	};

	struct ClipPlaneBufferType
	{
		D3DXVECTOR4 clipPlane;
	};

public:
	RefractionShaderClass();
	RefractionShaderClass(const RefractionShaderClass&);
	~RefractionShaderClass();

	bool Initialize( HWND);
	void ShutDown();
	bool Render( int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, D3DXVECTOR3, D3DXVECTOR4,D3DXVECTOR4, D3DXVECTOR4);

private:
	virtual bool InitializeShader( HWND, WCHAR*, WCHAR*);
	virtual void ShutdownShader();

	bool SetShaderParameters( D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, D3DXVECTOR3,
		D3DXVECTOR4, D3DXVECTOR4, D3DXVECTOR4);

private:
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_clipPlaneBuffer;

};

