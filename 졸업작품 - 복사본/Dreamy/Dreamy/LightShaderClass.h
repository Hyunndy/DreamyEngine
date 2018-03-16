////////////////////////////////////////////////////////////////////////////////
// Filename: LightShaderClass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ShaderClass.h"

/*
	셰이더: 실제 모델의 렌더링을 수행하는 작은 프로그램
	HLSL: 정점 셰이더, 픽셀 셰이더를 작성하는데 사용되는 언어

	LightShaderClass는 GPU상에 존재하는 3D 모델들을 그리는데 사용하는 HLSL셰이더(정점, 기하, 픽셀)를 호출하는 클래스이다.
	HLSL파일은 텍스트 파일로 존재한다.
	여기선 그걸 부르는것임!

	18.01.18
	컬러+텍스처 셰이더

	18.01.18
	컬러+텍스쳐+방향광(directional) + 정반사광(specular) 셰이더
	
*/
////////////////////////////////////////////////////////////////////////////////
// Class name: LightShaderClass
////////////////////////////////////////////////////////////////////////////////
class LightShaderClass : public ShaderClass
{
private:

	
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

	virtual bool Initialize(ID3D11Device*, HWND);
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR4, D3DXVECTOR3, D3DXVECTOR4, float);

private:

	virtual bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	virtual void ShutdownShader();
	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR4, D3DXVECTOR3, D3DXVECTOR4, float );


private:


	ID3D11Buffer* m_lightBuffer; // 방향광의 정보(색상과 방향)을 위한 상수 버퍼
								// 이 조명버퍼는 HLSL픽셀 셰이더에서 전역 조명 변수들을 설정하기 위해 사용된다.

	ID3D11Buffer* m_cameraBuffer; // 정반사광을 위해 정점 셰이더에서 카메라의 위치를 잡는 데 사용할 카메라 상수 버퍼

};

