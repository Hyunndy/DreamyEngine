////////////////////////////////////////////////////////////////////////////////
// Filename: TextureShaderClass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ShaderClass.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: TextureShaderClass
////////////////////////////////////////////////////////////////////////////////
class TextureShaderClass : public ShaderClass
{
private:


public:
	TextureShaderClass();
	TextureShaderClass(const TextureShaderClass&);
	~TextureShaderClass();


	virtual bool Initialize( HWND);
	bool Render( int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*);

private:
	virtual bool InitializeShader( HWND, WCHAR*, WCHAR*);
	virtual void ShutdownShader();

	bool SetShaderParameters( D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*);


};


/*
////////////////////////////////////////////////////////////////////////////////
// Filename: color.vs (정점 셰이더 : 완전 간단한 삼각형만 그리는)
////////////////////////////////////////////////////////////////////////////////

셰이더 프로그램은 전역 변수로부터 시작한다.
HLSL에서 대부분의 전역변수들은 Cbuffer(상수 버퍼)에 넣게 된다.
상수 버퍼를 조작하는 일은 셰이더의 효율적인 실행 뿐 아니라 그래픽카드가 어떻게 이 버퍼들을 저장하는지와 관련되기 때문에 매우 중요하다.
1) 오브젝트 마다(정점-월드,뷰,프로젝션 행렬) 2) 프레임 마다(라이팅) 3) 가끔 갱신되는(안개 등)으로 나눌 수 있다.

/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
matrix worldMatrix;
matrix viewMatrix;
matrix projectionMatrix;
};


//////////////
// TYPEDEFS //
//////////////

//정점버퍼에서 가져올 입력 구조체
struct VertexInputType
{
float4 position : POSITION;
float4 color : COLOR;
};

//픽셀셰이더(다음 렌더링파이프라인 단계)에 넘겨줄 출력 구조체
struct PixelInputType
{
float4 position : SV_POSITION;
float4 color : COLOR;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////

정점 셰이더는 정정 버퍼의 데이터를 처리하기 위해 GPU에 의해 호출된다.
정점 셰이더는 정점 버퍼의 모든 각 정점에 대해 호출된다.
정점 셰이더의 입력으로 들어오는것은 VertexInputType의 자료구조와 그 구조가 맞아 떨어져야 한다.

PixelInputType ColorVertexShader(VertexInputType input)
{
PixelInputType output;


// 올바르게 행렬 연산을 하기 위하여 position 벡터를 w까지 있는 4성분이 있는것으로 사용한다.
// 원래 position은 3차원인데 변환 행렬들이 4x4행렬이니까!
input.position.w = 1.0f;

// 정점의 위치를 월드, 뷰, 투영 순으로 계산해서 3D세계에 있는 정점을 우리의 시야에 맞는 2D화면의 위치로 지정해준다.
output.position = mul(input.position, worldMatrix);
output.position = mul(output.position, viewMatrix);
output.position = mul(output.position, projectionMatrix);

// 이 출력으로 나온 데이터들이 색상을 할당받은 뒤에 픽셀셰이더의 입력 데이터로 사용된다.
output.color = input.color;

return output;
}

*/


/*
픽셀 셰이더는 화면에 그려지는 도형의 각 픽셀들을 실제로 그린다.
정점 셰이더의 출력물을 픽셀 셰이더에서 사용한다.
////////////////////////////////////////////////////////////////////////////////
// Filename: color.ps (픽셀 셰이더)
////////////////////////////////////////////////////////////////////////////////



//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
float4 position : SV_POSITION;
float4 color : COLOR;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 ColorPixelShader(PixelInputType input) : SV_TARGET
{
return input.color;
}

*/