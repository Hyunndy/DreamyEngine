////////////////////////////////////////////////////////////////////////////////
// Filename: LightShaderClass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _LIGHTShaderClass_H_
#define _LIGHTShaderClass_H_


//////////////
// INCLUDES //
//////////////
#include "stdafx.h"
#include <d3dx11async.h>
#include <fstream>
using namespace std;

/*
	셰이더: 실제 모델의 렌더링을 수행하는 작은 프로그램
	HLSL: 정점 셰이더, 픽셀 셰이더를 작성하는데 사용되는 언어

	LightShaderClass는 GPU상에 존재하는 3D 모델들을 그리는데 사용하는 HLSL셰이더(정점, 기하, 픽셀)를 호출하는 클래스이다.
	HLSL파일은 텍스트 파일로 존재한다.
	여기선 그걸 부르는것임!

	18.01.18
	컬러+텍스처 셰이더

	18.01.18
	컬러+텍스쳐+방향광(directional) 셰이더
	
*/
////////////////////////////////////////////////////////////////////////////////
// Class name: LightShaderClass
////////////////////////////////////////////////////////////////////////////////
class LightShaderClass
{
private:

	// 정점 셰이더에서 사용될 cBuffer 구조체의 정의.
	// 셰이더가 올바른 렌더링을 하기 위해 이 구조체가 반드시 정점 셰이더에 사용된 구조체와 동일해야 한다.
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};	
	
	// 정점 셰이더 파일에 맞게 카메라 버퍼 구조체를 생성한다.
	// createbuffer함수가 실패하지 않도록 padding을 붙여 sizeof했을 때 16의 배수가 되도록한다.
	struct CameraBufferType
	{
		D3DXVECTOR3 cameraPosition;
		float padding; // 구조체의 크기가 16배수가 되게 하기 위해 안쓰는 변수 추가
					   // 이게 없으면 CreateBuffer가 무조건 16배수의 크기를 요구하기 때문에 실패한다.
	};
	// 조명 정보를 저장하기 위한 버퍼
	// 픽셀셰이더의 구조체 내용과 동일해야 한다.
	//LightBufferType 구조체는 픽셀 셰이더의 상수 버퍼와 마찬가지로 반사광의 색상과 강도를 저장하도록 바뀌었습니다.
	//주의해야 할 것은 기존의 16바이트의 배수 크기를 유지하기 위한 padding을 없애고 그 자리에 specularPower을 넣었다는 사실입니다. 
	//만약 padding을 넣지 않고 lightDirection 바로 밑에 specularColor을 넣었다면 셰이더가 올바로 동작하지 않았을 것입니다.
	//구조체의 크기가 16byte의 배수이긴 하지만 각 변수들이 16byte로 정렬되지 않았기 때문입니다
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

	// 셰이더의 초기화와 마무리를 제어한다.
	// Render함수는 셰이더에 사용되는 변수들을 설정하고 셰이더를 이용해 준비된 모델들의 정점들을 그려낸다.
	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR4, D3DXVECTOR3, D3DXVECTOR4, float);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR4, D3DXVECTOR3, D3DXVECTOR4, float );
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer; // 상수 버퍼

	ID3D11SamplerState* m_sampleState; // 텍스처의 샘플러 상태를 위한 포인터
									   // 텍스처 셰이더와의 인터페이스로서 사용된다.

	ID3D11Buffer* m_lightBuffer; // 방향광의 정보(색상과 방향)을 위한 상수 버퍼
								// 이 조명버퍼는 HLSL픽셀 셰이더에서 전역 조명 변수들을 설정하기 위해 사용된다.

	ID3D11Buffer* m_cameraBuffer; // 정반사광을 위해 정점 셰이더에서 카메라의 위치를 잡는 데 사용할 카메라 상수 버퍼

};

#endif

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