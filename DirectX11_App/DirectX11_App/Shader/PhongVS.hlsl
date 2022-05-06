cbuffer CBuf
{
    matrix modelView;       // 모델의 카메라 기준 좌표. (월드 * 뷰 변환)
    matrix modelViewProj;   // 모델의 투영 좌표. (월드 변환 * 뷰 변환 * 투영 변환)
};

struct VSOut
{
    float3 worldPos : Position;
    float3 normal : Normal;
    float2 tc : Texcoord;
    float4 pos : SV_Position;
};

VSOut main(float3 pos : Position, float3 n : Normal, float2 tc : Texcoord)
{
    VSOut vso;
    vso.worldPos = (float3) mul(float4(pos, 1.0f), modelView);
    vso.normal = mul(n, (float3x3) modelView);
    vso.pos = mul(float4(pos, 1.0f), modelViewProj);
    vso.tc = tc;
    return vso;
}