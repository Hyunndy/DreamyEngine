////////////////////////////////////////////////////////////////////////////////
// Filename: terrain.vs
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix lightViewMatrix;
    matrix lightProjectionMatrix;
};

//////////////////////////////////////////////////////////////////////////
//안개효과의 시작과 끝을 설정할 수 있도록 안개 상수 버퍼를 생성한다.
cbuffer FogBuffer
{
    float fogStart;
    float fogEnd;
}

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
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float3 color : COLOR;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float4 color : COLOR;
    //////////////////////////////////////////////////////////////////////////
    float fogFactor : FOG;  
    //////////////////////////////////////////////////////////////////////////
    float4 lightViewPosition : TEXCOORD1;
    float3 lightPos : TEXCOORD2; 
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType ShadowTerrainVertexShader(VertexInputType input)
{
    PixelInputType output;
    float4 worldPosition;

    //////////////////////////////////////////////////////////////////////////
    float4 cameraPosition;

	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	// 광원에 의해 보았을 때 정점의 위치를 계산한다.
    output.lightViewPosition = mul(input.position, worldMatrix);
    output.lightViewPosition = mul(output.lightViewPosition, lightViewMatrix);
    output.lightViewPosition = mul(output.lightViewPosition, lightProjectionMatrix);

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

	// Store the input color for the pixel shader to use.
    output.color = float4(input.color, 1.0f);

    //////////////////////////////////////////////////////////////////////////
    // 뷰 공간에서의 z값을 계산한다.
    // 안개의 시작과 끝 위치 정보를 안개 인자 공식에 넣어 픽셀 셰이더에 전달할 안개 인자를 계산한다.
    // ***이 안개 인자 공식으로 안개색의 농도를 달리할 수 있다***

    cameraPosition = mul(input.position, worldMatrix);
    cameraPosition = mul(cameraPosition, viewMatrix);

    output.fogFactor = saturate((fogEnd - cameraPosition.z) / (fogEnd - fogStart));
   // output.fogFactor = saturate( 1.0 / pow(2.71828,(cameraPosition.z * 1.0)));

       //world에서의 정점의 위치를 계산한다.
	worldPosition = mul(input.position, worldMatrix); 

    // 빛의 위치와 world의 정점 위치를 기반으로 빛의 위치를 결정한다.
    output.lightPos = lightPosition.xyz - worldPosition.xyz;

    // 빛의 위치 벡터를 정규화 한다.
    output.lightPos = normalize(output.lightPos);

    return output;
}