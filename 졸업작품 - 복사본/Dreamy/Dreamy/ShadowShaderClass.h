#pragma once
#include "ShaderClass.h"

/*-------------------------------------------------------------------------------------------------------------
이름: 그림자쉐이더
선행 개념: 
1) 텍스처 렌더링(RTT)
: 백버퍼가 아닌 텍스처에도 장면을 그릴 수 있게 한다.(거울, TV화면 등)

2) 깊이 버퍼(Z버퍼)
: 프러스텀안의 모든 픽셍의 깊이를 기록한다.
: 대부분의 비디오 카드는 32비트의 깊이 버퍼를 가진다.
: DepthStencil의 깊이 버퍼 형식이 DXGI_FORMAT_D24_UNORM_S8_UINT라고 설정된건 깊이버퍼=스텐실 버퍼라는 뜻, 깊이 채널은 24비트, 스텐실 채널의 경우 8비트로 설정한다는것이다.
: 깊이 버퍼의 값은 부동 소수점이다. 범위는 가까운 클리핑 평면에서 0.0~1.0이다.
: 하지만 선형 분포가 아니고, 부동 소수점 값의 약 90%는 가까운 클리핑 평면에 가까운 깊이 버퍼의 처음 10%에서 발생한다.
  나머지 10%(0.9부터 1.0까지)는 깊이 버퍼의 마지막 90%을 차지한다.
: 선형 분포가 아닌 이유는 대다수의 프로그램이 가까운 것은 정확하게, 멀리있는 것의 정밀도는 고려하지 않기 때문이다.

그림자매핑
: 빛의 관점에서 장면을 렌더링한다.
: 빛의 관점에서 렌더링 할 때 깊이 버퍼 정보를 텍스처 렌더링으로 렌더링한다. -> 깊이 정보로 채워진 이 텍스처를 그림자 맵이라고 한다.

*****************
: 장면을 렌더링 할 때 그림자 맵 텍스처를 다시 씬에 투영하여 그림자를 투사하는 객체의 깊이를 얻고 픽셀 쉐이더에서 픽셀 단위의 빛의 위치와 비교한다.
: 빛이 카메라에 더 가깝다는것을 알게되면 픽셀을 비추고, 객체가 카메라에 더 가깝다는것을 알게되면 픽셀을 그림자 처리한다.
*****************


-------------------------------------------------------------------------------------------------------------*/




class ShadowShaderClass :
	public ShaderClass
{
private:
	struct MatrixBufferType2
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
		D3DXMATRIX lightView;
		D3DXMATRIX lightProjection;
	};

	struct LightBufferType
	{
		D3DXVECTOR4 ambientColor;
		D3DXVECTOR4 diffuseColor;
	};

	struct LightBufferType2
	{
		D3DXVECTOR3 lightPosition;
		float padding;
	};


public:
	ShadowShaderClass();
	ShadowShaderClass(const ShadowShaderClass&);
	~ShadowShaderClass();

	virtual bool Initialize(HWND);
	bool Render( int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*,
		ID3D11ShaderResourceView*, D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR4);

private:
	virtual bool InitializeShader(HWND, WCHAR*, WCHAR*);
	virtual void ShutdownShader();
	bool SetShaderParameters( D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*,
		ID3D11ShaderResourceView*, D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR4);
	virtual void RenderShader(int);

private:
	ID3D11SamplerState* m_sampleStateWrap = nullptr;
	ID3D11SamplerState* m_sampleStateClamp = nullptr;
	
	ID3D11Buffer* m_lightBuffer = nullptr;
	ID3D11Buffer* m_lightBuffer2 = nullptr;
};

