Texture2D _map;
SamplerState samp;

cbuffer CameraBuffer : register(b0)
{
    matrix _view;
    matrix _projection;
};

cbuffer WorldBuffer : register(b1)
{
    matrix _world;
}

cbuffer BoneBuffer : register(b2)
{
    float4x4 _boneScale;
    float4x4 _boneArray[100];
    uint _skinning;
    float3 _bonePadding;
}

cbuffer SunBuffer : register(b0)
{
    float3 _lightDir;
    float _sunPadding;
};

struct VertexInput
{
    float4 position : POSITION0;
    float3 normal : NORMAL0;
    float2 uv : TEXCOORD0;
    float4 boneIndices : BLENDINDICES;
    float4 boneWeights : BLENDWEIGHT;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL0;
    float2 uv : TEXCOORD0;
};

PixelInput VS(VertexInput input)
{
    PixelInput output;

    if (_skinning == 0)
    {
       // output.position = mul(input.position, _world);
        output.position = input.position;
        output.normal = mul(input.normal, (float3x3) _world);
        output.normal = normalize(output.normal);
    }
    else
    {
        float4x4 skinTransform = 0;
        skinTransform += _boneArray[input.boneIndices.x] * input.boneWeights.x;
        skinTransform += _boneArray[input.boneIndices.y] * input.boneWeights.y;
        skinTransform += _boneArray[input.boneIndices.z] * input.boneWeights.z;
        skinTransform += _boneArray[input.boneIndices.w] * input.boneWeights.w;

        output.position = mul(input.position, skinTransform);
        output.normal = normalize(mul(input.normal, (float3x3) skinTransform));
    }
    
    output.position = mul(output.position, _boneScale);
    output.position = mul(output.position, _world); 
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);    

    output.uv = input.uv;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float3 light = _lightDir * -1;
    float intensity = saturate(dot(input.normal, light));
    float4 result = _map.Sample(samp, input.uv) * intensity;
    
    return saturate(result);
}