////////////////////////////////////////////////////////////////////////////////
// Filename: translate.vs
////////////////////////////////////////////////////////////////////////////////

// 반사 매핑 정점셰이더가 할 일
// 반사 매핑 에서는 텍스처의 픽셀마다의 알파값을 해당 픽셀에서의 빛의 세기로 활용하는 참조 테이블로서 사용한다.
// 참조 방법은 라이트 맵과 같지만 그 값을 정반사광으로 강조하는데 사용한다.
// 색상 텍스처 + 범프맵 텍스처 + 빛의세기 정보를 담고있는 반사 맵 텍스처


/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

// 이 셰이더는 정반사광을 표현해야 하므로 뷰 방향을 계산하기 위한 개념적인 카메라가 필요하다.
cbuffer CameraBuffer
{
    float3 cameraPosition;
}


//////////////
// TYPEDEFS //
//////////////

//노말맵 계산을 위해 tangent(접벡터), binormal(정점 법선 벡터)를 추가 한다.
// 반사광 연산을 위해 viewDirection을 가지게 한다.
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;

};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL; 
    float3 viewDirection : TEXCOORD1;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType TranslateVertexShader(VertexInputType input)
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
    
    // Calculate the normal vector against the world matrix only and then normalize the final value.
    output.normal = mul(input.normal, (float3x3)worldMatrix);
    output.normal = normalize(output.normal);

	// Calculate the tangent vector against the world matrix only and then normalize the final value.
    output.tangent = mul(input.tangent, (float3x3)worldMatrix);
    output.tangent = normalize(output.tangent);

    // Calculate the binormal vector against the world matrix only and then normalize the final value.
    output.binormal = mul(input.binormal, (float3x3)worldMatrix);
    output.binormal = normalize(output.binormal);

    //반사광 계산을 위해 계산된 뷰 방향을 픽셀셰이더에게 보내준다.
    // 월드 좌표계에서 정점의 위치를 계산한다.
    worldPosition = mul(input.position, worldMatrix);

    // 월드에서의 정점의 좌표와 카메라의 위치에 기초해 보는방향을 계산한다.
    output.viewDirection = cameraPosition.xyz - worldPosition.xyz;

    // 보는 방향 벡터를 정규화 시킨다.
    output.viewDirection = normalize(output.viewDirection);
    
    return output;
}