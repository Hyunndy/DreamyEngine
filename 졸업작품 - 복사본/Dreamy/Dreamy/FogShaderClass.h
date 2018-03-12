////////////////////////////////////////////////////////////////////////////////
// Filename: FogShaderClass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ShaderClass.h"

/*
	안개를 표현하기 위한 안개 셰이더.
	정점셰이더 안에 안개입자(fogfactor)을 계산하는 공식을 선언한다.

	정점셰이더에서 계산된 안개입자로 픽셀셰이더에서 안개농도와 안개색, 텍스처 컬러를 혼합 해 거리에 따른 텍스처의 최종색깔을 뽑아낸다.
*/

////////////////////////////////////////////////////////////////////////////////
// Class name: FogShaderClass
////////////////////////////////////////////////////////////////////////////////
class FogShaderClass : public ShaderClass
{
private:

	// 안개 버퍼 데이터를 위한 새 상수 버퍼 구조체.
	// 16바이트의 배수가 되기 위해 8바이트의 padding을 붙인다. (float:4바이트)
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

	// 셰이더의 초기화와 마무리를 제어한다.
	// Render함수는 셰이더에 사용되는 변수들을 설정하고 셰이더를 이용해 준비된 모델들의 정점들을 그려낸다.
	virtual bool Initialize(ID3D11Device*, HWND);
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, float, float);

private:

	virtual bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, float, float);


private:

	ID3D11Buffer* m_fogBuffer; // 안개 버퍼
};
