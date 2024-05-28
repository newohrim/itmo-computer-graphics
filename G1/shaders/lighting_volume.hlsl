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

cbuffer VertexConstantBuffer : register(b0)
{
    float4x4 worldTransform;
	float4x4 viewProj;
}

#define NR_CASCADES 4
cbuffer PixelConstantBuffer : register(b0)
{
	struct DirectionalLight
	{
		float4 mDirection;
		float4 mDiffuseColor;
		float4 mSpecColor;
	} dirLight;
	struct PointLight {    
    	float4 position;
    	float4 diffuse;
    	float4 specular;
		float constant;
    	float lin;
    	float quadratic;
		float _dummy;
	};
	#define NR_POINT_LIGHTS 4  
	PointLight pointLights[NR_POINT_LIGHTS];
	
	float4x4 viewMatr;
	float4 uCameraPos;
	float4 uAmbientLight;
	//float cascadePlaneDistances[NR_CASCADES]; doesn't work for whatever reason
	float4 cascadePlaneDistances;
	float uShininess; // TODO: remove from lighting
	int spotLightsNum;
}

cbuffer LightIndexBuffer : register(b2)
{
    // The index of the light in the Lights array.
    uint lightIdx;
}

Texture2D worldPositionsTex : register(t0);
Texture2D normalsTex : register(t1);
Texture2D albedoSpecTex : register(t2);
Texture2D depthStencilTex : register(t3);
Texture2DArray shadowMap : register(t4);

SamplerState samplerState : register(s0);

float4 CalcPointLight(PointLight light, float3 texVal, float3 normal, float3 fragPos, float3 viewDir, float specPower)
{
    float3 lightDir = normalize(light.position.xyz - fragPos);
    // диффузное освещение
    float3 diff = max(dot(normal, lightDir), 0.0);
    // освещение зеркальных бликов
    float3 reflectDir = reflect(-lightDir, normal);
    float spec = specPower * pow(max(dot(viewDir, reflectDir), 0.0), uShininess);
    // затухание
    float distance = length(light.position.xyz - fragPos);
    float attenuation = 1.0 / (light.constant + light.lin * distance + 
  			     light.quadratic * (distance * distance));
    // комбинируем результаты
    float3 diffuse = light.diffuse.xyz * diff * texVal;
    float3 specular = light.specular.xyz * spec * texVal;
    diffuse  *= attenuation;
    specular *= attenuation;
    return float4(diffuse + specular, 1.0);
} 

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
	float3 pixPos = float3((input.pos / 800.0f).xy, 0);
	//float4 worldPos = worldPositionsTex.Load(pixPos);
	float4 worldPos = worldPositionsTex.Sample(samplerState, pixPos);
	float4 normal = normalsTex.Sample(samplerState, pixPos);
	float4 albedoSpec = albedoSpecTex.Sample(samplerState, pixPos);

	// Surface normal
	float3 N = normalize(normal.xyz);
	// Vector from surface to light
	float3 L = normalize(-dirLight.mDirection.xyz);
	// Vector from surface to camera
	float3 V = normalize(uCameraPos.xyz - worldPos.xyz);
	// Reflection of -L about N
	float3 R = normalize(reflect(-L, N));

	// Compute phong reflection
	float3 Phong = uAmbientLight.xyz;
	float NdotL = dot(N, L);
	if (NdotL > 0)
	{
		//float3 Diffuse = dirLight.mDiffuseColor.xyz * NdotL;
		//float3 Specular = albedoSpec.w * dirLight.mSpecColor.xyz * pow(max(0.0, dot(R, V)), uShininess);
		//Phong += Diffuse + Specular;
	}
	
	//float4 col = float4(albedoSpec.xyz, 1.0f);
	
	// Final color is texture color times phong light (alpha = 1)
	//col *= float4(Phong, 1.0f);

	return CalcPointLight(pointLights[lightIdx], float4(albedoSpec.xyz, 1.0f), N, worldPos.xyz, V, albedoSpec.w); 

	//return col;
}
