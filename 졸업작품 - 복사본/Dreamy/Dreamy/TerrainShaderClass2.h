#pragma once
#include "ShaderClass.h"


/*
이름: TerrainShaderClass2
용도:
- 지형이 자체적인 특수 음영을 필요로하기 때문에 셰이더가 필요하다.
- 첫번째 지형셰이더: 확산(directional) + 주변(ambient)광 셰이더
- 두번째 지형셰이더: 텍스처 추가
*/


class TerrainShaderClass2 :
	public ShaderClass
{
private:

	/*
	typedef struct D3DXVECTOR4 {
	FLOAT x;
	FLOAT y;
	FLOAT z;
	FLOAT w;
	} D3DXVECTOR4;
	*/

	struct MatrixBufferType2
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
		D3DXMATRIX lightView;
		D3DXMATRIX lightProjection;
	};


	struct FogBufferType
	{
		float fogStart;
		float fogEnd;
		float padding1, padding2;
	};

	struct LightBufferType
	{
		D3DXVECTOR4 diffuseColor;
		D3DXVECTOR3 lightDirection;
		float padding;
		// D3D11_BIND_CONSTANT_BUFFER을 사용하면 BythWidth가 항상 16의 배수 여야 하며 그렇지 않으면 실패한다.
	};

	struct LightBufferType2
	{
		D3DXVECTOR3 lightPosition;
		float padding;
	};

public:
	TerrainShaderClass2();
	TerrainShaderClass2(const TerrainShaderClass2&);
	~TerrainShaderClass2();

	virtual bool Initialize(HWND);

	//bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX,  ID3D11ShaderResourceView*, ID3D11ShaderResourceView*,D3DXVECTOR3, D3DXVECTOR4);
	//bool Render(int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, D3DXVECTOR3, D3DXVECTOR4, float, float);
	bool Render(int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX
		, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, D3DXVECTOR3, D3DXVECTOR4, float, float);

	virtual void RenderShader(int);
	//bool SetShaderParameters(ID3D11DeviceContext*,int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*,  D3DXVECTOR3, D3DXVECTOR4);
	bool SetShaderParameters(int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX
							,ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*,D3DXVECTOR3, D3DXVECTOR4, float, float);

private:

	virtual bool InitializeShader(HWND, WCHAR*, WCHAR*);
	virtual void ShutdownShader();



private:
	ID3D11SamplerState* m_sampleStateWrap = nullptr;
	ID3D11SamplerState* m_sampleStateClamp = nullptr;

	ID3D11Buffer* m_lightBuffer = nullptr;
	ID3D11Buffer* m_lightBuffer2 = nullptr;


	ID3D11Buffer* m_fogBuffer; // 안개 버퍼
	//ID3D11Buffer* m_lightBuffer;


};


