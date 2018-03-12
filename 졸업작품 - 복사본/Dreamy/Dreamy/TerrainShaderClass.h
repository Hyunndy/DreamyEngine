#pragma once
#include "ShaderClass.h"


/*
�̸�: TerrainShaderClass
�뵵: 
- ������ ��ü���� Ư�� ������ �ʿ���ϱ� ������ ���̴��� �ʿ��ϴ�.
- ù��° �������̴�: Ȯ��(directional) + �ֺ�(ambient)�� ���̴�
- �ι�° �������̴�: �ؽ�ó �߰� 
*/


class TerrainShaderClass :
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


	struct LightBufferType
	{
		D3DXVECTOR4 diffuseColor;
		D3DXVECTOR3 lightDirection;
		float padding;
		// D3D11_BIND_CONSTANT_BUFFER�� ����ϸ� BythWidth�� �׻� 16�� ��� ���� �ϸ� �׷��� ������ �����Ѵ�.
	};

public:
	TerrainShaderClass();
	TerrainShaderClass(const TerrainShaderClass&);
	~TerrainShaderClass();

	virtual bool Initialize(ID3D11Device*, HWND);

	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX,  ID3D11ShaderResourceView*, ID3D11ShaderResourceView*,D3DXVECTOR3, D3DXVECTOR4);


	void RenderShader(ID3D11DeviceContext*, int);
	bool SetShaderParameters(ID3D11DeviceContext*,int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*,  D3DXVECTOR3, D3DXVECTOR4);

private:

	virtual bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	virtual void ShutdownShader();


private:

	ID3D11Buffer* m_lightBuffer;


};

