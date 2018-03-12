////////////////////////////////////////////////////////////////////////////////
// Filename: TextureShaderClass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ShaderClass.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: TextureShaderClass
////////////////////////////////////////////////////////////////////////////////
class TextureShaderClass : public ShaderClass
{
private:


public:
	TextureShaderClass();
	TextureShaderClass(const TextureShaderClass&);
	~TextureShaderClass();


	virtual bool Initialize(ID3D11Device*, HWND);
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*);

private:
	virtual bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	virtual void ShutdownShader();

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*);


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