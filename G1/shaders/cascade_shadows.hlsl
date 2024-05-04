#define NUMSPLITS 4

struct VS_IN
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};

struct GS_IN
{
    float4 pos : SV_POSITION;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    uint RTIndex : SV_RenderTargetArrayIndex; 
};

cbuffer VertexConstantBuffer : register(b0)
{
    float4x4 worldTransform;
	float4x4 viewProj;
}

cbuffer GeometryConstantBuffer : register(b0)
{
    float4x4 lightSpaceMatrices[NUMSPLITS];
}

PS_IN VSMain( VS_IN input )
{
	PS_IN output = (PS_IN)0;
	
	output.pos = mul(float4(input.pos, 1.0), worldTransform);
	
	return output;
}

[instance(NUMSPLITS)]
[maxvertexcount(3)]
void GSScene( triangle GS_IN input[3], in uint id : SV_GSInstanceID, inout TriangleStream<PS_IN> OutputStream)
{
    /*
    PSSceneIn output = (PSSceneIn)0;

    for( uint i=0; i<6; i+=2 )
    {
        output.Pos = input[i].Pos;
        output.Norm = input[i].Norm;
        output.Tex = input[i].Tex;
        
        OutputStream.Append( output );
    }
    
    OutputStream.RestartStrip();
    */

    /*
    PS_IN output = (PS_IN)0;

    for (int c = 0; c < 4; ++c) {
    for (int i = 0; i < 3; ++i) {
        output.pos = 
            mul(input[i].pos, lightSpaceMatrices[c]);
        output.RTIndex = c;
        OutputStream.Append( output );
    }
    OutputStream.RestartStrip();
    }
    */

    [unroll]
    for (int i = 0; i < 3; ++i) {
        PS_IN output = (PS_IN)0;
        output.pos = mul(float4(input[i].pos.xyz, 1.0f), lightSpaceMatrices[id]);
        output.RTIndex = id;
        OutputStream.Append(output);
    }
}

/*
void PSMain( PS_IN input ) : SV_Target
{
}
*/

/*
// Geometry shader output structure    
struct GS_OUT {   
    float4 pos : SV_POSITION;   
    uint RTIndex : SV_RenderTargetArrayIndex; 
};

// Geometry shader
[instance(NUMSPLITS)]
[maxvertexcount(NUMSPLITS * 3)]
void GS_RenderShadowMap(triangle VS_OUT In[3], inout TriangleStream<GS_OUT> triStream) {
    // For each split to render
    for(int split = firstSplit; split <= lastSplit; split++) {
        GS_OUT Out; // Set render target index.     
        Out.RTIndex = split; // For each vertex of triangle    
        for(int vertex = 0; vertex < 3; vertex++) {
            // Transform vertex with split-specific crop matrix.
            Out.pos = mul(In[vertex].pos, cropMatrix[split]); // Append vertex to stream
            triStream.Append(Out);
        }
        
        // Mark end of triangle
        triStream.RestartStrip();
    }
}
*/
