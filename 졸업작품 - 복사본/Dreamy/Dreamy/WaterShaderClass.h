#pragma once
#include "ShaderClass.h"


/*
이름: WaterShaderClass
용도: 
- water 정점셰이더, 픽셀셰이더를 통해 물을 렌더링한다.
- 물= 물 표면(반사) + 물 내부(굴절) + 노말맵(물결 가중치) -> 선형보간으로 색상을 뽑아낸다.
- 물 표면: 반사 텍스처(물의 높이, 카메라의 위치, 각도를 이용해 물 위의 모든것을 반사)
- 물 내부: 굴절 텍스처(반사의 반대)
- 물 = 반사텍스처+굴절텍스처의 선형보간으로 혼합

- 물 = 노말맵 타일링 2번(잔물결) + 반사텍스처 + 굴절텍스처 + 카메라 위치를 이용한 프리즘 + specular light 

- 반사쉐이더를 사용해 장면을 렌더링하지만 반전된 클립 평면을 사용해 물 아래의 모든 것을 렌더링하여 굴절을 만든다.
- 이것들은 모두 텍스처 렌더링으로 렌더링되므로 카메라의 시점을 기준으로 투영 텍스처를 사용하여 물 쿼드에 적용할 수 있다.
- 두 개의 개별적인 바둑판식으로 배열된 노멀맵 결과를 혼합해 잔물결을 애니메이션으로 표현한다.

*/
class WaterShaderClass :
	public ShaderClass
{
private:
	
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
		D3DXMATRIX reflection; // 이제 월드 프로젝션 반사 행렬을 만들어주기 때문에 ReflectionBufferType이 필요없다.
	};
	//프리즘, specular light을 이용하기 위한 camnormal버퍼.
	struct CamNormBufferType
	{
		D3DXVECTOR3 cameraPosition;
		float padding1;
		D3DXVECTOR2 normalMapTiling;
		D3DXVECTOR2 padding2;
	};

	//반사를 위해 월드 프로젝션 반사 행렬을 만들기 위한 반사 행렬.
//struct ReflectionBufferType
//{
//	D3DXMATRIX reflection;
//};

	//픽셀셰이더에서 물을 그려주기 위한 물 버퍼
	struct WaterBufferType
	{
		D3DXVECTOR4 refractionTint;
		D3DXVECTOR3 lightDirection;
		float waterTranslation;
		float reflectRefractScale;
		float specularShininess;
		D3DXVECTOR2 padding;
	};


public:
	WaterShaderClass();
	WaterShaderClass(const WaterShaderClass&);
	~WaterShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*,
		/*ID3D11ShaderResourceView*,*/ D3DXVECTOR3, D3DXVECTOR2, float, float, D3DXVECTOR4, D3DXVECTOR3, float);


private:
	virtual bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	virtual void ShutdownShader();


	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*,
		/*ID3D11ShaderResourceView*,*/ D3DXVECTOR3, D3DXVECTOR2, float, float, D3DXVECTOR4, D3DXVECTOR3, float);

private:
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_camNormBuffer;
	ID3D11Buffer* m_waterBuffer;
	
};

