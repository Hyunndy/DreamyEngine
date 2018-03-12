////////////////////////////////////////////////////////////////////////////////
// Filename: fog.vs
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

//안개효과의 시작과 끝을 설정할 수 있도록 안개 상수 버퍼를 생성한다.
cbuffer FogBuffer
{
    float fogStart;
    float fogEnd;
}

//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
};

// 안개 인자를 픽셀 셰이더에서 받을 수 있게 한다.
// 안개의 위치에 따라 안개색의 농도를 달리하여 픽셀셰이더에서 텍스처 색과 더하게 한다.
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float fogFactor : FOG;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType FogVertexShader(VertexInputType input)
{
    PixelInputType output;
    float4 cameraPosition;
    

	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;

    // 뷰 공간에서의 z값을 계산한다.
    // 안개의 시작과 끝 위치 정보를 안개 인자 공식에 넣어 픽셀 셰이더에 전달할 안개 인자를 계산한다.
    // ***이 안개 인자 공식으로 안개색의 농도를 달리할 수 있다***

    cameraPosition = mul(input.position, worldMatrix);
    cameraPosition = mul(cameraPosition, viewMatrix);

    output.fogFactor = saturate((fogEnd - cameraPosition.z) / (fogEnd - fogStart));
   // output.fogFactor = saturate( 1.0 / pow(2.71828,(cameraPosition.z * 1.0)));
    return output;
}