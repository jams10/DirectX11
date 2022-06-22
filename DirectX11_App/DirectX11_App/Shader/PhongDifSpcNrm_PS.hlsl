#include "ShaderOps.hlsli"
#include "LightVectorData.hlsli"
#include "PointLight.hlsli"

cbuffer ObjectCBuf
{
    bool useGlossAlpha;
    bool useSpecularMap;
    float3 specularColor;
    float specularWeight;
    float specularGloss;
    bool useNormalMap;
    float normalMapWeight;
};

Texture2D tex;
Texture2D spec;
Texture2D nmap;

SamplerState splr;

float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float3 viewTan : Tangent, float3 viewBitan : Bitangent, float2 tc : Texcoord) : SV_Target
{
    // sample diffuse texture
    float4 dtex = tex.Sample(splr, tc);
    
    #ifdef MASK_BOI
    // bail if highly translucent
    clip(dtex.a < 0.1f ? -1 : 1);
    
    // flip normal when backface
    if (dot(viewNormal, viewFragPos) >= 0.0f)
    {
        viewNormal = -viewNormal;
    }
    #endif
    
    // �޽��� �븻 ���� ����ȭ
    viewNormal = normalize(viewNormal);
    // �븻���� ������ ��� �븻�� �ؽ��ĸ� ���ø� �Ͽ� �븻���� ������, �� �븻 ���� ���� �����.
    if (useNormalMap)
    {
        const float3 mappedNormal = MapNormal(normalize(viewTan), normalize(viewBitan), viewNormal, tc, nmap, splr);
        viewNormal = lerp(viewNormal, mappedNormal, normalMapWeight);
    }
    
    // ǥ���� ��Ÿ���� fragment���� ������ ���ϴ� ����
    const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewFragPos);
    // specular �Ķ���͸� ������.(�̹��� ���� ����� ������ ������ ��� ���� ����� ������)
    float3 specularReflectionColor;
    float specularPower = specularGloss;
    const float4 specularSample = spec.Sample(splr, tc);
    if (useSpecularMap)
    {
        specularReflectionColor = specularSample.rgb;
    }
    else
    {
        specularReflectionColor = specularColor;
    }
    if (useGlossAlpha)
    {
        specularPower = pow(2.0f, specularSample.a * 13.0f);
    }
    // ���� ��
    const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
    // ���ݻ� ��
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, viewNormal);
    // �ݻ�� specular 
    const float3 specularReflected = Speculate(
        diffuseColor * diffuseIntensity * specularReflectionColor, specularWeight, viewNormal,
        lv.vToL, viewFragPos, att, specularPower
    );

    // ���ݻ� �ؽ��� ������ ���� ����� ���ݻ�, �ֺ��� ���� �ݻ�� ���ݻ籤�� ������.
    return float4(saturate((diffuse + ambient) * dtex.rgb + specularReflected), 1.0f);
}