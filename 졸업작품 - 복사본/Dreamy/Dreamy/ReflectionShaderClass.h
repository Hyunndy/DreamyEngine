#pragma once
#include "ShaderClass.h"

/*
	�ݻ� ���̴� Ŭ����.
	�ؽ�ó���̴��� �ݻ� �� ��İ� �ݻ� �ؽ��ĸ� �����ϴ� ����� �߰��Ǿ���.
*/


class ReflectionShaderClass :
	public ShaderClass
{
private:

	//�ݻ� �� ����� ���� ���� ��� ����
	struct ReflectionBufferType
	{
		D3DXMATRIX reflectionMatrix;
	};

public:
	ReflectionShaderClass();
	ReflectionShaderClass(const ReflectionShaderClass&);
	~ReflectionShaderClass();


	virtual bool Initialize(ID3D11Device*, HWND);
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*,D3DXMATRIX);

private:
	virtual bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	virtual void ShutdownShader();

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, D3DXMATRIX);

	ID3D11Buffer* m_reflectionBuffer;
};

