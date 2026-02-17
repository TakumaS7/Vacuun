
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register(b0)
{
    matrix WorldViewProjection;
}

// マテリアルバッファ
cbuffer MaterialBuffer : register(b1)
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Emission;
    float Shininess;
    float3 Dummy; //16bit境界用
}


//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD0;
    float4 Color : COLOR;
};



Texture2D		g_Texture : register( t0 );
SamplerState	g_SamplerState : register( s0 );

//--------------------------------------------------------------------------------------
// Nothing special here
void VertexShaderPolygon(in float4 inPosition : POSITION0,
						  in float4 inNormal : NORMAL0,
						  in float4 inDiffuse : COLOR0,
						  in float2 inTexCoord : TEXCOORD0,

						  out float4 outPosition : SV_POSITION,
						  out float4 outNormal : NORMAL0,
						  out float2 outTexCoord : TEXCOORD0,
						  out float4 outDiffuse : COLOR0)
{
    outPosition = mul(inPosition, WorldViewProjection);

    outNormal = inNormal;
    outTexCoord = inTexCoord;

    outDiffuse = inDiffuse * Diffuse;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------

float3 YUVToRGB(float3 yuv) {
  // BT.601 coefs
  static const float3 yuvCoef_r = { 1.164f, 0.000f, 1.596f };
  static const float3 yuvCoef_g = { 1.164f, -0.392f, -0.813f };
  static const float3 yuvCoef_b = { 1.164f, 2.017f, 0.000f };
  yuv -= float3(0.0625f, 0.5f, 0.5f);
  return saturate(float3( 
    dot( yuv, yuvCoef_r ),
    dot( yuv, yuvCoef_g ),
    dot( yuv, yuvCoef_b )
    ));
}

void PS(
    in float4 inPosition : SV_POSITION0,
    in float4 inNormal : NORMAL0,
    in float2 inTexCoord : TEXCOORD0,
    in float4 inDiffuse : COLOR0,

    out float4 outDiffuse : SV_Target)
{
    float y = g_Texture.Sample(g_SamplerState, float2(inTexCoord.x, inTexCoord.y * 0.5)).r;
    float u = g_Texture.Sample(g_SamplerState, float2(inTexCoord.x * 0.5, 0.50 + inTexCoord.y * 0.25)).r;
    float v = g_Texture.Sample(g_SamplerState, float2(inTexCoord.x * 0.5, 0.75 + inTexCoord.y * 0.25)).r;

    float3 rgb = YUVToRGB(float3(y, u, v));
    
    // 緑色を透過（G成分が大きい場合 Alpha を 0 にする）
    float alpha = (rgb.r <= 0.1f && rgb.g >= 0.9f && rgb.b <= 0.1f) ? 0.0f : 1.0f;


    outDiffuse = float4(rgb, alpha);

}
