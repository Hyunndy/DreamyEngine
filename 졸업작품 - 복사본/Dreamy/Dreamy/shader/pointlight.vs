////////////////////////////////////////////////////////////////////////////////
// Filename: pointlight.vs
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////

//HLSL에서는 #define으로 배열을 생성할 수 있다.
#define NUM_LIGHTS 1


cbuffer MatrixBuffer
{ 
	matrix worldMatrix; 
	matrix viewMatrix; 
	matrix projectionMatrix;
};


cbuffer LightPositionBuffer
{
	float4 lightPosition[NUM_LIGHTS];
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

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	//점조명의 갯수에 따라 lightPos를 만들어준다.
	float3 lightPos1 : TEXCOORD1;
	//float3 lightPos2 : TEXCOORD2;
	//float3 lightPos3 : TEXCOORD3; 
	//float3 lightPos4 : TEXCOORD4;

};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType PointLightVertexShader(VertexInputType input)
{
    PixelInputType output;
    float4 worldPosition;

	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    

	output.tex = input.tex;
    
	//월드 행렬과 반대인 노말 벡터를 계산한다.
    output.normal = mul(input.normal, (float3x3)worldMatrix);

    // Calculate the position of the vertex in the world.
    worldPosition = mul(input.position, worldMatrix);	
	
	//노말벡터를 정규화 시킨다.
    output.normal = normalize(output.normal);


	 //월드에서 정점의 위치와 빛의위치를 기반으로 최종 포인트라이트 위치를 계산한다.
	 output.lightPos1.xyz = lightPosition[0].xyz - worldPosition.xyz; 
	// output.lightPos2.xyz = lightPosition[1].xyz - worldPosition.xyz; 
	// output.lightPos3.xyz = lightPosition[2].xyz - worldPosition.xyz;
	// output.lightPos4.xyz = lightPosition[3].xyz - worldPosition.xyz; 
	 
	 // light포지션을 정규화시킨다.
	 output.lightPos1 = normalize(output.lightPos1); 
	// output.lightPos2 = normalize(output.lightPos2); 
	// output.lightPos3 = normalize(output.lightPos3); 
	// output.lightPos4 = normalize(output.lightPos4);


    return output;
}