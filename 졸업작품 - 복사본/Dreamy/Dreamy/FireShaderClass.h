#pragma once
#include "ShaderClass.h"

/*------------------------------------------------------------------------------------------------------------------------
이름: FireShaderClass
용도:
- 준비물: 1) 회색조의 노이즈 텍스쳐(일렁임) 2) 불의 색상으로 구성된 노이즈와 화염의 경계가 보이는 텍스쳐(색) 3) 불꽃의 형태를 위한 알파텍스쳐(모양)
- 노이즈 텍스쳐를 이용해 물,유리,얼음 등과 같이 텍스쳐의 샘플링 위치를 흩뜨려 놓는다.

- 셰이더 동작 순서
 1) 노이즈 텍스쳐를 가져다가 전부 다르게 스케일링해 세 개의 서로 다른 텍스쳐를 생성한다.
  *이것들은 단순히 원본 텍스처를 반복 나열하여 디테일한 텍스쳐를 만든 것이므로 "옥타브"라고 부른다.
  *이 노이즈 텍스쳐들을 위쪽으로 스크롤시켜 불꽃이 위쪽으로 일렁이는듯한 느낌을 준다.
  *세 개의 속도를 다르게 해서 나중에 혼합한다.

 2) (0,1)범위의 값인 노이즈 텍스쳐들을 (-1,+1)로 변환한다.
  *변환 시 일부 노이즈가 사라지고 좀 더 불 꽃 모양에 가깝게 된다.
  *모든 노이즈 텍스처를 (-1,+1)로 변환 시 각 텍스쳐의 x축과 y축을 왜곡시킬 수 있다.
  *노이즈 텍스쳐들이 왜곡되고 나면 3개의 텍스쳐를 전부 혼합해 최종 노이즈 텍스쳐 모양을 만든다.
  **왜곡이 오직 x,y축에 이루어지기 때문에 샘플링 위치를 정하기 위한 룩업 테이블 처럼 사용될것이다.
  **세 노이즈 텍스쳐 모두 프레임 마다 서로 다른 속도로 위쪽 스크롤되기 때문에 이것들을 혼합하게 되면 흐르는 듯한 노이즈가 만들어지게 된다.

 3) 원본 불꽃 색상 텍스쳐를 흩뜨린다.
  *노이즈 텍스처를 사용해 텍스처 샘플링 위치를 바꾼다.
  *2)번에서 노이즈 자체의 샘플링 위치를 흩뜨렸다.
  *왜곡 스케일과 여백 값을 사용해 왜곡이 텍스쳐 위쪽에선 더 크게, 아래로 갈수록 적게 일어나도록 한다. 
  -> 노이즈 역시 텍스처 아래쪽부터 시작해 y축을 올라가면서 위로 갈수록 더 큰 값으로 왜곡한다.
  *왜곡된 최종 노이즈 텍스쳐를 룩업 테이블 처럼 사용하여 불꽃 색상 텍스처를 샘플링한다.
  **색상 텍스처 샘플링 시 Wrap대신 Clamp를 사용해 바닥쪽이 망가지지 않게 한다.
 
 4) 3번 까지 불타오르는 모양의 사각형을 생성했으므로 더욱 불꽃 모양이 나오게 알파 텍스처를 사용한다.
  *흩뜨려 놓은 노이즈 텍스처를 사용해 알파 텍스처를 샘플링하면 알파 텍스처를 타오르는 불꽃처럼 보이게 만들 수 있다.
  *샘플링 시 wrap대신 clamp를 사용해 불꽃의 바닥 주위에서 텍스처의 위쪽을 사용하는 것을 방지해야 한다.

  5) 이펙트를 완성하기 위해 만들어낸 알파 값을 불꽃 사각형의 알파 채널에 사용하여 알파블렌딩으로 마무리될 수 있게 한다.
------------------------------------------------------------------------------------------------------------------------*/


class FireShaderClass :
	public ShaderClass
{
private:
	
	// fire.vs에 있는 NoiseBuffer.
	// 노이즈를 계산하기 위한 변수를 포함한다.
	struct NoiseBufferType
	{
		float frameTime;
		D3DXVECTOR3 scrollSpeeds;
		D3DXVECTOR3 scales;
		float padding;
	};


	// fire.ps에 있는 불꽃 효과를 만들 때 노이즈 텍스처 값들을 흩뜨리기 위한 값들이 있는 DistortionBuffer.
	struct DistortionBufferType
	{
		D3DXVECTOR2 distortion1;
		D3DXVECTOR2 distortion2;
		D3DXVECTOR2 distortion3;
		float distortionScale;
		float distortionBias;
	};



public:
	FireShaderClass();
	FireShaderClass(const FireShaderClass&);
	~FireShaderClass();


	virtual bool Initialize(ID3D11Device*, HWND);
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*,
				float, D3DXVECTOR3, D3DXVECTOR3, D3DXVECTOR2, D3DXVECTOR2, D3DXVECTOR2, float, float);

private:

	virtual bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);

	void ShutdownShader();
	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*,
		ID3D11ShaderResourceView*, float, D3DXVECTOR3, D3DXVECTOR3, D3DXVECTOR2, D3DXVECTOR2, D3DXVECTOR2, float, float);

private:
	ID3D11Buffer* m_noiseBuffer;
	ID3D11SamplerState* m_sampleState2; // wrap대신 clamp를 사용하는 sampleState2.
	ID3D11Buffer* m_distortionBuffer;
};

