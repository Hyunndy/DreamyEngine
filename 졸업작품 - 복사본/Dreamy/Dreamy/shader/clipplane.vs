////////////////////////////////////////////////////////////////////////////////
// Filename: clipplane.vs
////////////////////////////////////////////////////////////////////////////////

// DX9에선 함수로 클리핑평면을 만들 수 있었는데,
// DX10/11에선 픽셀 셰이더의 입력 구조체에 SV_ClipDistance이란 시맨킥을 넣어 사용한다.
// 기본 텍스처 셰이더에 한 개의 클리핑 평면을 다룰 수 있도록 한다.

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

// 클리핑 평면의 부동소수점 벡터 4개를 가지고 있으므로 외부 오브젝트는 클리핑 평면이 어디에 있는지, 평면의 어느쪽이 클리핑 될것인지 정할 수 있다.
// 4번째 숫자는 위치를 지정한다. 벡터의 값이 (0.0f, 1.0f, 0.0f, -5.0f)라면 이 평면은 y축 아래에 있는 모든것들을 잘라내고,
// 높이 5.0부터 시작한다. (0.0f, -1.0f, 0.0f, 0.0f)이면 y축 위로는 그리지 않는다는 것이다.

cbuffer ClipPlaneBuffer

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
PixelInputType SpecMapVertexShader(VertexInputType input)
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