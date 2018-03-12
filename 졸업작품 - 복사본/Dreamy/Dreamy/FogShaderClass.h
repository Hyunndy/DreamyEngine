////////////////////////////////////////////////////////////////////////////////
// Filename: FogShaderClass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ShaderClass.h"

/*
	�Ȱ��� ǥ���ϱ� ���� �Ȱ� ���̴�.
	�������̴� �ȿ� �Ȱ�����(fogfactor)�� ����ϴ� ������ �����Ѵ�.

	�������̴����� ���� �Ȱ����ڷ� �ȼ����̴����� �Ȱ��󵵿� �Ȱ���, �ؽ�ó �÷��� ȥ�� �� �Ÿ��� ���� �ؽ�ó�� ���������� �̾Ƴ���.
*/

////////////////////////////////////////////////////////////////////////////////
// Class name: FogShaderClass
////////////////////////////////////////////////////////////////////////////////
class FogShaderClass : public ShaderClass
{
private:

	// �Ȱ� ���� �����͸� ���� �� ��� ���� ����ü.
	// 16����Ʈ�� ����� �Ǳ� ���� 8����Ʈ�� padding�� ���δ�. (float:4����Ʈ)
	struct FogBufferType
	{
		float fogStart;
		float fogEnd;
		float padding1, padding2;
	};


public:
	FogShaderClass();
	FogShaderClass(const FogShaderClass&);
	~FogShaderClass();

	// ���̴��� �ʱ�ȭ�� �������� �����Ѵ�.
	// Render�Լ��� ���̴��� ���Ǵ� �������� �����ϰ� ���̴��� �̿��� �غ�� �𵨵��� �������� �׷�����.
	virtual bool Initialize(ID3D11Device*, HWND);
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, float, float);

private:

	virtual bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, float, float);


private:

	ID3D11Buffer* m_fogBuffer; // �Ȱ� ����
};
