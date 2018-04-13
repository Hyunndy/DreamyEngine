////////////////////////////////////////////////////////////////////////////////
// Filename: light.vs
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////

// 01.22 
// ���ݻ籤 ����� ���� ������ ��ġ,ī�޶��� ��ġ�� �˾ƾ� �Ѵ�.
// ���� ī�޶� ����, viewdirection�� �߰����ش�!


cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer CameraBuffer
{
	float3 cameraPosition;
	float padding;
};

//////////////
// TYPEDEFS //
//////////////

// 3���������� ���� ���͸� ���� �ִ�.
// ���� ���ʹ� ������ �޽� ǥ���� ������ �Ի��� ������ ���� �� �ֱ� ������ ǥ�� ������ �� �ʿ��ϴ�.
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	// ���� ���̴����� ���� ������ ����ؾ� �ϱ� ������ viewDirection�� �߰��ϰ� �ȼ� ���̴��� �Ѱ�����.
	// view(���¹��� ����) = ī�޶��� ��ġ ���� - ������ ��ġ ����
	float3 viewDirection : TEXCOORD1;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType LightVertexShader(VertexInputType input)
{
    PixelInputType output;
    float4 worldPosition;

	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;
    
	// �� ���� ���ʹ� ���� ��ǥ�迡�� ���ǰ� �ȼ� ���̴��� ���Է¿� �������� ����ȭ�ȴ�.
    output.normal = mul(input.normal, (float3x3)worldMatrix);
	
    // Normalize the normal vector.
    output.normal = normalize(output.normal);

	
	/* 01.22 ���ݻ籤���� ���� �߰�*/

	// �������̴��� ���⿡�� ���� ����(view)���Ͱ� ���ȴ�.
	// ������ ������� ��ġ�� ����ϰ� ī�޶��� ��ġ���� �� ��ġ�� �����ν� ������ � �������� �������� �˾Ƴ���.
	// �� ����� ����� ����ȭ�Ǿ� �ȼ����̴��� ��������. (view����!)

	    worldPosition = mul(input.position, worldMatrix); //������ ��ġ
	    output.viewDirection = cameraPosition.xyz - worldPosition.xyz; // ī�޶��� ��ġ - ������ ��ġ
	    output.viewDirection = normalize(output.viewDirection); // ����ȭ�Ǿ� �ȼ����̴��� �Ѱ�����.


    return output;
}