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
    output.position = float4(input.position, 1.0f);
    output.color = input.color;
    return output;
}