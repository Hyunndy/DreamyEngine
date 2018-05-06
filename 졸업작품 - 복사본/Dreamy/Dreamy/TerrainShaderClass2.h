#pragma once
#include "ShaderClass.h"


/*
�̸�: TerrainShaderClass2
�뵵:
- ������ ��ü���� Ư�� ������ �ʿ���ϱ� ������ ���̴��� �ʿ��ϴ�.
- ù��° �������̴�: Ȯ��(directional) + �ֺ�(ambient)�� ���̴�
- �ι�° �������̴�: �ؽ�ó �߰�
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
		// D3D11_BIND_CONSTANT_BUFFER�� ����ϸ� BythWidth�� �׻� 16�� ��� ���� �ϸ� �׷��� ������ �����Ѵ�.
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


	ID3D11Buffer* m_fogBuffer; // �Ȱ� ����
	//ID3D11Buffer* m_lightBuffer;


};


