////////////////////////////////////////////////////////////////////////////////
// Filename: shadow.vs
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////


// 정점을 빛의 관점에서 변환시키기 위해 viewmatrix, projectionmatrix가 필요하다.
cbuffer MatrixBuffer2
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix lightViewMatrix;
    matrix lightProjectionMatrix;
};

cbuffer LightBuffer2
{
    float3 lightPosition;
    float padding;
};

//////////////
// TYPEDEFS //
//////////////

struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};


// 픽셀셰이더에 빛의 관점으로 전환된 정점을 보내기 위해 lightViewPosition을 가진다.
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float4 lightViewPosition : TEXCOORD1;
    float3 lightPos : TEXCOORD2;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType ShadowVertexShader(VertexInputType input)
{
    PixelInputType output;
    float4 worldPosition;

	// 적절한 행렬 계산을 위해 벡터를 4단위로 변경한다.
    input.position.w = 1.0f;

	// 정점의 scene에서의 위치를 계산한다.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	// 광원에 의해 보았을 때 정점의 위치를 계산한다.
    output.lightViewPosition = mul(input.position, worldMatrix);
    output.lightViewPosition = mul(output.lightViewPosition, lightViewMatrix);
    output.lightViewPosition = mul(output.lightViewPosition, lightProjectionMatrix);


	// 픽셀 쉐이더의 텍스쳐 좌표를 저장한다.
	output.tex = input.tex;
    
	// 월드 행렬에 대해서만 법선 벡터를 계산한다.
    output.normal = mul(input.normal, (float3x3)worldMatrix);
	
    // 법선 벡터를 정규화 한다.
    output.normal = normalize(output.normal);

    //world에서의 정점의 위치를 계산한다.
	worldPosition = mul(input.position, worldMatrix); 

    // 빛의 위치와 world의 정점 위치를 기반으로 빛의 위치를 결정한다.
    output.lightPos = lightPosition.xyz - worldPosition.xyz;

    // 빛의 위치 벡터를 정규화 한다.
    output.lightPos = normalize(output.lightPos);

    return output;	
}