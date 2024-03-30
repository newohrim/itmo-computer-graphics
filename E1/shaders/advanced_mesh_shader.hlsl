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

cbuffer PixelConstantBuffer : register(b0)
{
	struct DirectionalLight
	{
		float4 mDirection;
		float4 mDiffuseColor;
		float4 mSpecColor;
	} dirLight;
	float4 color;
	float4 uCameraPos;
	float4 uAmbientLight;
    float uSpecPower;
	float3 dummy;
	bool isTextureSet;
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
	
	output.worldPos = mul(float4(input.pos, 1.0), worldTransform);
	output.pos = mul(float4(input.pos, 1.0), mul(worldTransform, viewProj));
	output.normal = mul(float4(input.normal, 0.0), worldTransform);
	output.uv = input.uv;
	
	return output;
}

float4 PSMain( PS_IN input ) : SV_Target
{
	/*
	float4 albedo = tex.Sample(samplerState, input.uv);
	clip(albedo.a - 0.01);
	float3 normal = normalize(input.normal.xyz);
	float3 viewDir = normalize(uCameraPos.xyz - input.worldPos.xyz);
	float3 lightDir = -dirLight.mDirection.xyz;
	float3 refVec = normalize(reflect(lightDir, normal));
	float3 diffuse = max(0, dot(lightDir, normal)) * dirLight.mDiffuseColor.xyz;
	float3 specular = dirLight.mSpecColor.xyz * pow(max(0.0, dot(refVec, viewDir)), uSpecPower);
	
	return albedo * float4(uAmbientLight.xyz + diffuse + specular, 1.0);
	*/
	
	// Surface normal
	float3 N = normalize(input.normal.xyz);
	// Vector from surface to light
	float3 L = normalize(-dirLight.mDirection.xyz);
	// Vector from surface to camera
	float3 V = normalize(uCameraPos.xyz - input.worldPos.xyz);
	// Reflection of -L about N
	float3 R = normalize(reflect(-L, N));

	// Compute phong reflection
	float3 Phong = uAmbientLight.xyz;
	float NdotL = dot(N, L);
	if (NdotL > 0)
	{
		float3 Diffuse = dirLight.mDiffuseColor.xyz * NdotL;
		float3 Specular = 0.0;
		if (uSpecPower > 0) {
			Specular = dirLight.mSpecColor.xyz * pow(max(0.0, dot(R, V)), uSpecPower);
		}
		Phong += Diffuse + Specular;
	}
	
	float4 col = color;
	if (isTextureSet) {
		col = tex.Sample(samplerState, input.uv);
	}
	
	// Final color is texture color times phong light (alpha = 1)
    return col * float4(Phong, 1.0f);
}
