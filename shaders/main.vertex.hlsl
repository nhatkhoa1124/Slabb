cbuffer TransformCB : register(b0)
{
    matrix mvp_matrix;
}

struct VSInput
{
    float3 position  : POSITION;
    float4 color     : COLOR0;
    float3 normal    : NORMAL0;
    float2 tex_coord : TEXCOORD0;
};

struct VSOutput
{
    float4 position  : SV_POSITION;
    float4 color     : COLOR0;
    float3 normal    : NORMAL0;
    float2 tex_coord : TEXCOORD0;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;

    output.position = mul(float4(input.position, 1.0f), mvp_matrix);

    output.color = input.color;
    output.normal = input.normal;
    output.tex_coord = input.tex_coord;

    return output;
}
