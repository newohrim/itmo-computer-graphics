struct VS_IN
{
	float3 pos : POSITION0;
	float3 normal : NORMAL0;
	float2 uv : TEXCOORD0;
};

struct PS_IN
{
	float4 pos : SV_POSITION;
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

PS_IN VSMain( VS_IN input )
{
	PS_IN output = (PS_IN)0;
	
	output.pos = mul(float4(input.pos, 1.0), mul(worldTransform, viewProj));
	
	return output;
}

float4 PSMain( PS_IN input ) : SV_Target
{
	return color;
}
