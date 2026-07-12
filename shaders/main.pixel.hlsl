struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR0;
    float3 normal : NORMAL0; // 🟢 ADDED: Realigns register slot 2!
    float2 tex_coord : TEXCOORD0; // 🟢 Pushed down to register slot 3 to match VSOutput
};

Texture2D base_color_tex : register(t0);
SamplerState linear_sampler : register(s0);

float4 PSMain(PSInput input) : SV_Target
{
    // Sampling remains exactly the same, but now the registers line up perfectly!
    float4 sampled = base_color_tex.Sample(linear_sampler, input.tex_coord);
    return sampled * input.color;
}