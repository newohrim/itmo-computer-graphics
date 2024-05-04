struct VS_IN
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};

struct PS_IN
{
	float4 pos : SV_POSITION;
	float4 normal : NORMAL;
	float4 worldPos : POSITION;
	float2 uv : TEXCOORD;
};

struct PS_OUTPUT
{
    float3 position: SV_Target0;
    float3 normal: SV_Target1;
    float4 albedoSpec: SV_Target2;
};

cbuffer VertexConstantBuffer : register(b0)
{
    float4x4 worldTransform;
	float4x4 viewProj;
}

cbuffer PixelConstantBuffer : register(b0)
{
	float4 color;
	float4 uCameraPos;
    float uSpecPower;
	int isTextureSet;
}

Texture2D tex : register(t0);
SamplerState samplerState : register(s0);

PS_IN VSMain( VS_IN input )
{
	PS_IN output = (PS_IN)0;
	
	output.worldPos = mul(float4(input.pos, 1.0), worldTransform);
	output.pos = mul(float4(input.pos, 1.0), mul(worldTransform, viewProj));
	output.normal = mul(float4(input.normal, 0.0), worldTransform);
	output.uv = input.uv;
	
	return output;
}

PS_OUTPUT PSMain( PS_IN input ) : SV_Target
{
    PS_OUTPUT output;

    // Surface normal
	float3 N = normalize(input.normal.xyz);
	// Vector from surface to camera
	float3 V = normalize(uCameraPos.xyz - input.worldPos.xyz);

    output.position = input.worldPos.xyz;
    output.normal = N;
    output.albedoSpec = isTextureSet * tex.Sample(samplerState, input.uv) + !isTextureSet * float4(color.xyz, 0.0);
    output.albedoSpec.a = uSpecPower;

    return output;
}
