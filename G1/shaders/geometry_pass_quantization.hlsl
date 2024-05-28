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

	
	uint mask = 1 << 5;
	float resX = input.pos.x - fmod(input.pos.x, mask);
	float resY = input.pos.y - fmod(input.pos.y, mask);
	float resZ = input.pos.z - fmod(input.pos.z, mask);
	output.pos = mul(float4(resX, resY, resZ, 1.0), mul(worldTransform, viewProj));
	
	//output.pos = mul(float4(input.pos, 1.0), mul(worldTransform, viewProj));
	output.normal = mul(float4(input.normal, 0.0), worldTransform);

	/*
	float resUVX = input.uv.x - fmod(input.uv.x, mask);
	float resUVY = input.uv.y - fmod(input.uv.y, mask);
	output.uv = float2(resUVX, resUVY);
	*/

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
	
	/*
	uint mask = 1 << 1;
	float resX = input.worldPos.x - fmod(input.worldPos.x, mask);
	float resY = input.worldPos.y - fmod(input.worldPos.y, mask);
	float resZ = input.worldPos.z - fmod(input.worldPos.z, mask);
	output.position = float3(resX, resY, resZ);
	*/
	output.position = input.worldPos.xyz;
    output.normal = N;
    output.albedoSpec = isTextureSet * tex.Sample(samplerState, input.uv) + !isTextureSet * float4(color.xyz, 0.0);
    output.albedoSpec.a = uSpecPower;

    return output;
}
