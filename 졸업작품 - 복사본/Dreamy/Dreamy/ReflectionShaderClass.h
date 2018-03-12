#pragma once
#include "ShaderClass.h"

/*
	반사 셰이더 클래스.
	텍스처셰이더에 반사 뷰 행렬과 반사 텍스쳐를 제어하는 기능이 추가되었다.
*/


class ReflectionShaderClass :
	public ShaderClass
{
private:

	//반사 뷰 행렬을 위한 동적 상수 버퍼
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

