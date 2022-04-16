cbuffer CBuf
{
    matrix model;           // ���� ���� ���� ��ǥ. (���� ��ȯ)
    matrix modelViewProj;   // ���� ���� ��ǥ. (���� ��ȯ * �� ��ȯ * ���� ��ȯ)
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
    vso.normal = mul(n, (float3x3) model);                  // �븻 ������ ��� translation�� �������� ���� ���̱⿡ float3x3�� ĳ����.
    vso.pos = mul(float4(pos, 1.0f), modelViewProj);
    return vso;
}