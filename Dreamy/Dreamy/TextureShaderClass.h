////////////////////////////////////////////////////////////////////////////////
// Filename: TextureShaderClass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TEXTUREHADERCLASS_H_
#define _TEXTURETextureShaderClass_H_


//////////////
// INCLUDES //
//////////////
#include "stdafx.h"
#include <d3dx11async.h>
#include <fstream>
using namespace std;

/*
���̴�: ���� ���� �������� �����ϴ� ���� ���α׷�
HLSL: ���� ���̴�, �ȼ� ���̴��� �ۼ��ϴµ� ���Ǵ� ���

TextureShaderClass�� GPU�� �����ϴ� 3D �𵨵��� �׸��µ� ����ϴ� HLSL���̴�(����, ����, �ȼ�)�� ȣ���ϴ� Ŭ�����̴�.
HLSL������ �ؽ�Ʈ ���Ϸ� �����Ѵ�.
���⼱ �װ� �θ��°���!

18.01.18
�÷�+�ؽ�ó ���̴�

18.01.18
�÷�+�ؽ���+���Ɽ(directional) ���̴�

*/
////////////////////////////////////////////////////////////////////////////////
// Class name: TextureShaderClass
////////////////////////////////////////////////////////////////////////////////
class TextureShaderClass
{
private:

	// ���� ���̴����� ���� cBuffer ����ü�� ����.
	// ���̴��� �ùٸ� �������� �ϱ� ���� �� ����ü�� �ݵ�� ���� ���̴��� ���� ����ü�� �����ؾ� �Ѵ�.
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};


public:
	TextureShaderClass();
	TextureShaderClass(const TextureShaderClass&);
	~TextureShaderClass();

	// ���̴��� �ʱ�ȭ�� �������� �����Ѵ�.
	// Render�Լ��� ���̴��� ���Ǵ� �������� �����ϰ� ���̴��� �̿��� �غ�� �𵨵��� �������� �׷�����.
	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer; // ��� ����

	ID3D11SamplerState* m_sampleState; // �ؽ�ó�� ���÷� ���¸� ���� ������
									   // �ؽ�ó ���̴����� �������̽��μ� ���ȴ�.



};

#endif

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