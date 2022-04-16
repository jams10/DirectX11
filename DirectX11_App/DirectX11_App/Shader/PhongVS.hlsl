cbuffer CBuf
{
    matrix model;           // 모델의 월드 공간 좌표. (월드 변환)
    matrix modelViewProj;   // 모델의 투영 좌표. (월드 변환 * 뷰 변환 * 투영 변환)
};

struct VSOut
{
    float3 worldPos : Position;
    float3 normal : Normal;
    float4 pos : SV_Position;
};

VSOut main(float3 pos : Position, float3 n : Normal)
{
    VSOut vso;
    vso.worldPos = (float3) mul(float4(pos, 1.0f), model);
    vso.normal = mul(n, (float3x3) model);                  // 노말 벡터의 경우 translation을 적용하지 않을 것이기에 float3x3로 캐스팅.
    vso.pos = mul(float4(pos, 1.0f), modelViewProj);
    return vso;
}