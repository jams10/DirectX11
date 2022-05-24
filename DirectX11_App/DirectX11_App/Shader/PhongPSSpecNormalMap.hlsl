#include "ShaderOps.hlsli"
#include "LightVectorData.hlsli"
#include "PointLight.hlsli"

cbuffer ObjectCBuf
{
    bool normalMapEnabled;
    bool specularMapEnabled;
    bool hasGloss;
    float specularPowerConst;
    float3 specularColor;
    float specularMapWeight;
};

Texture2D tex;
Texture2D spec;
Texture2D nmap;

SamplerState splr;

float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float3 viewTan : Tangent, float3 viewBitan : Bitangent, float2 tc : Texcoord) : SV_Target
{
    // �޽��� �븻 ���� ����ȭ
    viewNormal = normalize(viewNormal);
    // �븻���� ������ ��� �븻�� �ؽ��ĸ� ���ø� �Ͽ� �븻���� ������, �� �븻 ���� ���� �����.
    if (normalMapEnabled)
    {
        viewNormal = MapNormal(normalize(viewTan), normalize(viewBitan), viewNormal, tc, nmap, splr);
    }
    
    // ǥ���� ��Ÿ���� fragment���� ������ ���ϴ� ����
    const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewFragPos);
    // specular �Ķ���͸� ������.(�̹��� ���� ����� ������ ������ ��� ���� ����� ������)
    float3 specularReflectionColor;
    float specularPower = specularPowerConst;
    if (specularMapEnabled)
    {
        const float4 specularSample = spec.Sample(splr, tc);
        specularReflectionColor = specularSample.rgb * specularMapWeight;
        if (hasGloss)
        {
            specularPower = pow(2.0f, specularSample.a * 13.0f);
        }
    }
    else
    {
        specularReflectionColor = specularColor;
    }
    // ���� ��
    const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
    // ���ݻ� ��
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, viewNormal);
    // �ݻ�� specular 
    const float3 specularReflected = Speculate(
        specularReflectionColor, 1.0f, viewNormal,
        lv.vToL, viewFragPos, att, specularPower
    );
    // ���ݻ� �ؽ��� ������ ���� ����� ���ݻ�, �ֺ��� ���� �ݻ�� ���ݻ籤�� ������.
    return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specularReflected), 1.0f);
}