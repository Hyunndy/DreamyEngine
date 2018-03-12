////////////////////////////////////////////////////////////////////////////////
// Filename: LightShaderClass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ShaderClass.h"

/*
	���̴�: ���� ���� �������� �����ϴ� ���� ���α׷�
	HLSL: ���� ���̴�, �ȼ� ���̴��� �ۼ��ϴµ� ���Ǵ� ���

	LightShaderClass�� GPU�� �����ϴ� 3D �𵨵��� �׸��µ� ����ϴ� HLSL���̴�(����, ����, �ȼ�)�� ȣ���ϴ� Ŭ�����̴�.
	HLSL������ �ؽ�Ʈ ���Ϸ� �����Ѵ�.
	���⼱ �װ� �θ��°���!

	18.01.18
	�÷�+�ؽ�ó ���̴�

	18.01.18
	�÷�+�ؽ���+���Ɽ(directional) + ���ݻ籤(specular) ���̴�
	
*/
////////////////////////////////////////////////////////////////////////////////
// Class name: LightShaderClass
////////////////////////////////////////////////////////////////////////////////
class LightShaderClass : public ShaderClass
{
private:

	
	// ���� ���̴� ���Ͽ� �°� ī�޶� ���� ����ü�� �����Ѵ�.
	// createbuffer�Լ��� �������� �ʵ��� padding�� �ٿ� sizeof���� �� 16�� ����� �ǵ����Ѵ�.
	struct CameraBufferType
	{
		D3DXVECTOR3 cameraPosition;
		float padding; // ����ü�� ũ�Ⱑ 16����� �ǰ� �ϱ� ���� �Ⱦ��� ���� �߰�
					   // �̰� ������ CreateBuffer�� ������ 16����� ũ�⸦ �䱸�ϱ� ������ �����Ѵ�.
	};
	// ���� ������ �����ϱ� ���� ����
	// �ȼ����̴��� ����ü ����� �����ؾ� �Ѵ�.
	//LightBufferType ����ü�� �ȼ� ���̴��� ��� ���ۿ� ���������� �ݻ籤�� ����� ������ �����ϵ��� �ٲ�����ϴ�.
	//�����ؾ� �� ���� ������ 16����Ʈ�� ��� ũ�⸦ �����ϱ� ���� padding�� ���ְ� �� �ڸ��� specularPower�� �־��ٴ� ����Դϴ�. 
	//���� padding�� ���� �ʰ� lightDirection �ٷ� �ؿ� specularColor�� �־��ٸ� ���̴��� �ùٷ� �������� �ʾ��� ���Դϴ�.
	//����ü�� ũ�Ⱑ 16byte�� ����̱� ������ �� �������� 16byte�� ���ĵ��� �ʾұ� �����Դϴ�
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


	ID3D11Buffer* m_lightBuffer; // ���Ɽ�� ����(����� ����)�� ���� ��� ����
								// �� ������۴� HLSL�ȼ� ���̴����� ���� ���� �������� �����ϱ� ���� ���ȴ�.

	ID3D11Buffer* m_cameraBuffer; // ���ݻ籤�� ���� ���� ���̴����� ī�޶��� ��ġ�� ��� �� ����� ī�޶� ��� ����

};



/*
////////////////////////////////////////////////////////////////////////////////
// Filename: color.vs (���� ���̴� : ���� ������ �ﰢ���� �׸���)
////////////////////////////////////////////////////////////////////////////////

	���̴� ���α׷��� ���� �����κ��� �����Ѵ�.
	HLSL���� ��κ��� ������������ Cbuffer(��� ����)�� �ְ� �ȴ�.
	��� ���۸� �����ϴ� ���� ���̴��� ȿ������ ���� �� �ƴ϶� �׷���ī�尡 ��� �� ���۵��� �����ϴ����� ���õǱ� ������ �ſ� �߿��ϴ�.
	1) ������Ʈ ����(����-����,��,�������� ���) 2) ������ ����(������) 3) ���� ���ŵǴ�(�Ȱ� ��)���� ���� �� �ִ�.

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

//�������ۿ��� ������ �Է� ����ü
struct VertexInputType
{
float4 position : POSITION;
float4 color : COLOR;
};

//�ȼ����̴�(���� ���������������� �ܰ�)�� �Ѱ��� ��� ����ü 
struct PixelInputType
{
float4 position : SV_POSITION;
float4 color : COLOR;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
	
	���� ���̴��� ���� ������ �����͸� ó���ϱ� ���� GPU�� ���� ȣ��ȴ�.
	���� ���̴��� ���� ������ ��� �� ������ ���� ȣ��ȴ�.
	���� ���̴��� �Է����� �����°��� VertexInputType�� �ڷᱸ���� �� ������ �¾� �������� �Ѵ�.

PixelInputType ColorVertexShader(VertexInputType input)
{
PixelInputType output;


// �ùٸ��� ��� ������ �ϱ� ���Ͽ� position ���͸� w���� �ִ� 4������ �ִ°����� ����Ѵ�. 
// ���� position�� 3�����ε� ��ȯ ��ĵ��� 4x4����̴ϱ�!
input.position.w = 1.0f;

// ������ ��ġ�� ����, ��, ���� ������ ����ؼ� 3D���迡 �ִ� ������ �츮�� �þ߿� �´� 2Dȭ���� ��ġ�� �������ش�.
output.position = mul(input.position, worldMatrix);
output.position = mul(output.position, viewMatrix);
output.position = mul(output.position, projectionMatrix);

// �� ������� ���� �����͵��� ������ �Ҵ���� �ڿ� �ȼ����̴��� �Է� �����ͷ� ���ȴ�.
output.color = input.color;

return output;
}

*/


/*
	�ȼ� ���̴��� ȭ�鿡 �׷����� ������ �� �ȼ����� ������ �׸���.
	���� ���̴��� ��¹��� �ȼ� ���̴����� ����Ѵ�.
////////////////////////////////////////////////////////////////////////////////
// Filename: color.ps (�ȼ� ���̴�)
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