////////////////////////////////////////////////////////////////////////////////
// Filename: fire.vs
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

// 매 프레임마다 계산한 값들을 저장할 NoiseBuffer.
cbuffer NoiseBuffer
{
    float frameTime; // 셰이더가 노이즈 텍스처들을 y축 방향으로 스크롤 할 수 있게 하는 frame 간격.
    float3 scrollSpeeds; // 3개의 노이즈 텍스쳐에 각각의 스피드를 넣어주기 위한 3개짜리 배열.
    float3 scales; // 3개의 노이즈 텍스처의 서로 다른 스케일값(=옥타브값)
    float padding; //cbuffer의 16의 배수로 맞추기 위한 float.
};

//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float2 texCoords1 : TEXCOORD1;
    float2 texCoords2 : TEXCOORD2;
    float2 texCoords3 : TEXCOORD3;      
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType FireVertexShader(VertexInputType input)
{
    PixelInputType output;
    

	// 올바른 행렬 연산을 하기 위해 위치 벡터의 성분이 4개가 되도록 한다.
    input.position.w = 1.0f;

	// 정점의 위치를 월드, 뷰, 투영 행렬에 대해 각각 계산한다.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	// 픽셀 셰이더를 위해 텍스처 좌표를 저장해둔다.
    output.tex = input.tex;
    

    // 세 개의 다른 노이즈 텍스처 샘플링 좌표값을 만들어내고 이를 이용해 세 개의 다른 노이즈 텍스처를 생성할 수 있다.
    // 각 텍스처 좌표마다 스케일 값을 곱해주어 그 값에 따라 텍스처가 타일링되게 한다.
    // 그리고 나서 frameTime, scrollSpeeds 배열의 값들을 사용해 세 개의 텍스처의 y좌표값들이 스크롤되게 한다.
    // scrollSpeed배열의 값들을 다 다르게 하면 불꽃에 공간감이 더해진다.

    output.texCoords1 = (input.tex * scales.x);
    output.texCoords1.y = output.texCoords1.y + (frameTime * scrollSpeeds.x);

    output.texCoords2 = (input.tex * scales.y);
    output.texCoords2.y = output.texCoords2.y + (frameTime * scrollSpeeds.y);

    output.texCoords3 = (input.tex * scales.z);
    output.texCoords3.y = output.texCoords3.y + (frameTime * scrollSpeeds.z);    


    return output;
}