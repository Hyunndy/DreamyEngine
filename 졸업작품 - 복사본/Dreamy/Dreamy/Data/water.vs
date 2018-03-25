////////////////////////////////////////////////////////////////////////////////
// Filename: water.vs
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
    matrix reflectionMatrix;    
};

//이제 카메라가 reflectionMatrix해주니까 카메라 노말 버퍼를 추가하자 ㅡㅡ.
cbuffer CamNormBuffer
{
    float3 cameraPosition;
    float padding1;
    float2 normalMapTiling;
    float2 padding2;
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
   // float4 reflectionPosition : TEXCOORD0;
    float4 refractionPosition : TEXCOORD1;
    float3 viewDirection : TEXCOORD2;
    float2 tex1 : TEXCOORD3;
    float2 tex2 : TEXCOORD4;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType WaterVertexShader(VertexInputType input)
{
    PixelInputType output;
    matrix reflectProjectWorld; // 반사용 월드 프로젝션 행렬
    matrix viewProjectWorld; // 굴절을 위한 뷰 행렬
    float4 worldPosition;   

	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    


    // 프로젝션(투영) 행렬을 cbuffer에 넣어준 반사 행렬에 곱해준 다음, 이 행렬을 world에 곱해줘서 반사용 월드 프로젝션 행렬을 생성한다.
    reflectProjectWorld = mul(reflectionMatrix, projectionMatrix);
    reflectProjectWorld = mul(worldMatrix, reflectProjectWorld);

    //이게 텍스처 좌표라는걸 기억하자.
    //반사 텍스처 좌표.
    //output.reflectionPosition = mul(input.position, reflectProjectWorld);

    // 굴절용 변환 행렬. 반사 행렬과의 차이점은 이건 cbuffer에 넣어준 반사행렬이 아니라 view행렬 이라는 것이다.
    viewProjectWorld = mul(viewMatrix, projectionMatrix);
    viewProjectWorld = mul(worldMatrix, viewProjectWorld);

    //굴절 텍스처 좌표.
    output.refractionPosition = mul(input.position, viewProjectWorld);


    //프리즌이랑 정반사광 계산을 위해 카메라의 뷰 방향을 계산해야한다.
    // Calculate the position of the vertex in the world.
    worldPosition = mul(input.position, worldMatrix);

    // Determine the viewing direction based on the position of the camera and the position of the vertex in the world.
    output.viewDirection = cameraPosition.xyz - worldPosition.xyz;

    // Normalize the viewing direction vector.
    output.viewDirection = normalize(output.viewDirection); 


    // 호수 쿼드에 몇번이나 곱해질 노말맵을 위해 텍스처 샘플 좌표를 계산해준다.
    output.tex1 = input.tex / normalMapTiling.x;
    output.tex2 = input.tex / normalMapTiling.y;


    return output;
}