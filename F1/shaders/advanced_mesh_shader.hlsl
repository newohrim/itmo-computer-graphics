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
	float4 color;
	float4 uCameraPos;
	float4 uAmbientLight;
	//float cascadePlaneDistances[NR_CASCADES]; doesn't work for whatever reason
	float4 cascadePlaneDistances;
    float uSpecPower;
	float uShininess;
	int spotLightsNum;
	int isTextureSet;
}

cbuffer Cascades : register(b1) 
{
	float4x4 lightSpaceMatrices[NR_CASCADES];
}

cbuffer VertexConstantBuffer : register(b0)
{
    float4x4 worldTransform;
	float4x4 viewProj;
}

Texture2D tex : register(t0);
Texture2DArray shadowMap : register(t1);
SamplerState samplerState : register(s0);

float4 CalcPointLight(PointLight light, float3 texVal, float3 normal, float3 fragPos, float3 viewDir)
{
    float3 lightDir = normalize(light.position.xyz - fragPos);
    // диффузное освещение
    float3 diff = max(dot(normal, lightDir), 0.0);
    // освещение зеркальных бликов
    float3 reflectDir = reflect(-lightDir, normal);
    float spec = uSpecPower * pow(max(dot(viewDir, reflectDir), 0.0), uShininess);
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
	// select cascade layer
	float4 fragPosViewSpace = mul(float4(input.worldPos.xyz, 1.0), viewMatr);
	float depthValue = abs(fragPosViewSpace.z);
	//return float4(depthValue / 100.0, depthValue / 100.0, depthValue / 100.0, 1.0);
	
	int layer = 4;
	for (int i = 0; i < NR_CASCADES; ++i)
	{
		if (depthValue < cascadePlaneDistances[i])
		{
			layer = i;
			break;
		}
	}

	// Surface normal
	float3 N = normalize(input.normal.xyz);
	// Vector from surface to light
	float3 L = normalize(-dirLight.mDirection.xyz);
	// Vector from surface to camera
	float3 V = normalize(uCameraPos.xyz - input.worldPos.xyz);
	// Reflection of -L about N
	float3 R = normalize(reflect(-L, N));

	float4 fragPosLightSpace = mul(float4(input.worldPos.xyz, 1.0), lightSpaceMatrices[layer]);
	// perform perspective divide
	float3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// transform to [0,1] range
	projCoords = projCoords * 0.5 + 0.5;
		
	// get depth of current fragment from light's perspective
	float currentDepth = projCoords.z;
	if (currentDepth  > 1.0)
	{
		return 0.0;
	}
	// calculate bias (based on depth map resolution and slope)
	float bias = max(0.05 * (1.0 - dot(N, L)), 0.005);
	if (layer == NR_CASCADES)
	{
		float farPlane = 100.0;
		bias *= 1 / (farPlane * 0.5f);
	}
	else
	{
		bias *= 1 / (cascadePlaneDistances[layer] * 0.5f);
	}

	// PCF
	float shadow = 0.0;
	//float2 texelSize = 1.0 / float2(textureSize(shadowMap, 0));
	float2 texelSize = 1.0 / float2(2000.0, 2000.0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = shadowMap.Sample(samplerState,
						float3(projCoords.xy + float2(x, y) * texelSize,
						layer)
						).r; 
			shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	shadow /= 9.0;
		
	// keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
	if(projCoords.z > 1.0)
	{
		shadow = 0.0;
	}
	
	//return float4((float)layer / 5.0, (float)layer / 5.0, (float)layer / 5.0, 1.0);

	// Compute phong reflection
	float3 Phong = uAmbientLight.xyz;
	float NdotL = dot(N, L);
	if (NdotL > 0)
	{
		float3 Diffuse = dirLight.mDiffuseColor.xyz * NdotL;
		float3 Specular = uSpecPower * dirLight.mSpecColor.xyz * pow(max(0.0, dot(R, V)), uShininess);
		Phong += (1.0 - shadow) * (Diffuse + Specular);
	}
	
	float4 col = color;
	float4 texVal = 0;
	if (isTextureSet == 1) {
		col = texVal = tex.Sample(samplerState, input.uv);
	}
	
	// Final color is texture color times phong light (alpha = 1)
	col *= float4(Phong, 1.0f);

	int spotLightsNumClipped = min(spotLightsNum, NR_POINT_LIGHTS);
	for (int i = 0; i < spotLightsNum; i++) {
		col += CalcPointLight(pointLights[i], texVal, N, input.worldPos.xyz, V); 
	}

	return col;
}
