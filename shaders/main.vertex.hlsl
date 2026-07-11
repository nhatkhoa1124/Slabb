cbuffer TransformCB : register(b0)
{
    matrix mvp_matrix;
}

struct VSInput
{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct VSOut
{
    float4 position : SV_Position;
    float4 color : COLOR;
};

VSOut VSMain(VSInput input)
{
    VSOut output;
    output.position = mul(float4(input.position, 1.0f), mvp_matrix);
    output.color = input.color;
    return output;
}