////////////////////////////////////////////////////////////////////////////////
// Filename: light.vs
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////

// 01.22 
// 정반사광 계산을 위해 정점의 위치,카메라의 위치를 알아야 한다.
// 따라서 카메라 버퍼, viewdirection를 추가해준다!


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

// 3차원벡터인 법선 벡터를 갖고 있다.
// 법선 벡터는 광선이 메시 표면의 점으로 입사한 각도를 구할 수 있기 때문에 표면 법선이 꼭 필요하다.
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
	// 정점 셰이더에서 보는 방향을 계산해야 하기 때문에 viewDirection을 추가하고 픽셀 셰이더로 넘겨진다.
	// view(보는방향 벡터) = 카메라의 위치 벡터 - 정점의 위치 벡터
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
    
	// 이 법선 벡터는 월드 좌표계에서 계산되고 픽셀 셰이더으 ㅣ입력에 들어가기전에 정규화된다.
    output.normal = mul(input.normal, (float3x3)worldMatrix);
	
    // Normalize the normal vector.
    output.normal = normalize(output.normal);

	
	/* 01.22 정반사광으로 인해 추가*/

	// 정점셰이더는 여기에서 보는 방향(view)벡터가 계산된다.
	// 정점의 월드상의 위치를 계산하고 카메라의 위치에서 이 위치를 뺌으로써 실제로 어떤 방향으로 보는지를 알아낸다.
	// 이 계산의 결과는 정규화되어 픽셀셰이더로 보내진다. (view벡터!)

	    worldPosition = mul(input.position, worldMatrix); //정점의 위치
	    output.viewDirection = cameraPosition.xyz - worldPosition.xyz; // 카메라의 위치 - 정점의 위치
	    output.viewDirection = normalize(output.viewDirection); // 정규화되어 픽셀셰이더로 넘겨진다.


    return output;
}