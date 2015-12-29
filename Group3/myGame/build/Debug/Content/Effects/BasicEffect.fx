/************* Resources *************/

cbuffer CBufferPerObject
{
    float4x4 WorldViewProjection : WORLDVIEWPROJECTION; 
}

/************* Data Structures *************/

struct VS_INPUT
{
    float4 ObjectPosition: POSITION;
    float4 Color : COLOR;
};

struct VS_OUTPUT 
{
    float4 Position: SV_Position;
    float4 Color : COLOR;
};

RasterizerState DisableCulling
{
    CullMode = NONE;
};

/************* Vertex Shader *************/

VS_OUTPUT vertex_shader(VS_INPUT IN)
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;
    
    OUT.Position = mul(IN.ObjectPosition, WorldViewProjection);
    OUT.Color = IN.Color;

	OUT.Color[0] = mul(OUT.Color, OUT.Position);
	OUT.Color[1] = mul(OUT.Color, OUT.Position);
	OUT.Color[2] = mul(OUT.Color, OUT.Position);
    
    return OUT;
}

/************* Pixel Shader *************/

float4 pixel_shader(VS_OUTPUT IN) : SV_Target
{

    return IN.Color;
}

/************* Techniques *************/

technique11 main11
{
    pass p0
    {
        SetVertexShader(CompileShader(vs_5_0, vertex_shader()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, pixel_shader()));

        SetRasterizerState(DisableCulling);
    }
}