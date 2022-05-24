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
    // 메쉬의 노말 값을 정규화
    viewNormal = normalize(viewNormal);
    // 노말맵을 적용할 경우 노말맵 텍스쳐를 샘플링 하여 노말값을 얻어오고, 이 노말 벡터 값을 사용함.
    if (normalMapEnabled)
    {
        viewNormal = MapNormal(normalize(viewTan), normalize(viewBitan), viewNormal, tc, nmap, splr);
    }
    
    // 표면을 나타내는 fragment에서 광원을 향하는 벡터
    const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewFragPos);
    // specular 파라미터를 결정함.(이미지 맵을 사용할 것인지 정해진 상수 값을 사용할 것인지)
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
    // 감쇠 값
    const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
    // 난반사 값
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, viewNormal);
    // 반사된 specular 
    const float3 specularReflected = Speculate(
        specularReflectionColor, 1.0f, viewNormal,
        lv.vToL, viewFragPos, att, specularPower
    );
    // 난반사 텍스쳐 색상을 통해 감쇠된 난반사, 주변광 색상에 반사된 정반사광을 더해줌.
    return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specularReflected), 1.0f);
}