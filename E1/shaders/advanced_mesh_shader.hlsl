struct VS_IN
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};

struct PS_IN
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
};

cbuffer PixelConstantBuffer : register(b0)
{
    float4 color;
}

cbuffer VertexConstantBuffer : register(b0)
{
    float4x4 worldTransform;
	float4x4 viewProj;
}

Texture2D tex : register(t0);
SamplerState samplerState : register(s0);

PS_IN VSMain( VS_IN input )
{
	PS_IN output = (PS_IN)0;
	
	output.pos = mul(float4(input.pos, 1.0), mul(worldTransform, viewProj));
	output.uv = input.uv;
	
	return output;
}

float4 PSMain( PS_IN input ) : SV_Target
{
	//return color;
	return tex.Sample(samplerState, input.uv);
}
