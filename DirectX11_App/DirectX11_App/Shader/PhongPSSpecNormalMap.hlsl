cbuffer LightCBuf
{
    float3 viewLightPos;
    float3 ambient;
    float3 diffuseColor;
    float diffuseIntensity;
    float attConst;
    float attLin;
    float attQuad;
};

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

float3 MapNormal(
    const in float3 tan,
    const in float3 bitan,
    const in float3 normal,
    const in float2 tc,
    uniform Texture2D nmap,
    uniform SamplerState splr)
{
    // 접선 공간에서 타겟 공간(tan/bitan/normal)으로 변환해주는 행렬 생성.
    const float3x3 tanToTarget = float3x3(tan, bitan, normal);
    // 노말 맵으로 부터 노말 값 얻어온 뒤에 노말 값 범위에 맞는 값으로 수정해줌.
    const float3 normalSample = nmap.Sample(splr, tc).xyz;
    const float3 tanNormal = normalSample * 2.0f - 1.0f;
    // 얻어온 노말 값을 타겟 공간으로 변환.
    return normalize(mul(tanNormal, tanToTarget));
}

float Attenuate(uniform float attConst, uniform float attLin, uniform float attQuad, const in float distFragToL)
{
    return 1.0f / (attConst + attLin * distFragToL + attQuad * (distFragToL * distFragToL));
}

float3 Diffuse( // 난반사값 계산 함수.
    uniform float3 diffuseColor,
    uniform float diffuseIntensity,
    const in float att,
    const in float3 viewDirFragToL,
    const in float3 viewNormal)
{
    return diffuseColor * diffuseIntensity * att * max(0.0f, dot(viewDirFragToL, viewNormal));
}

float3 Speculate( // 정반사값 계산 함수.
    const in float3 specularColor,
    uniform float specularIntensity,
    const in float3 viewNormal,
    const in float3 viewFragToL,
    const in float3 viewPos,
    const in float att,
    const in float specularPower)
{
    // 반사광 벡터를 계산.
    const float3 w = viewNormal * dot(viewFragToL, viewNormal);
    const float3 r = normalize(w * 2.0f - viewFragToL);
    // 카메라에서 물체의 표면을 나타내는 fragment를 향하는 벡터 (뷰 공간)
    const float3 viewCamToFrag = normalize(viewPos);
    // 시야 벡터와 반사 벡터 사이의 각도를 기준으로 한 specular 요소 색상 값을 계산함. 제곱 함수로 specular 범위를 좁힘.
    return att * specularColor * specularIntensity * pow(max(0.0f, dot(-r, viewCamToFrag)), specularPower);
}

float4 main(float3 viewPos : Position, float3 viewNormal : Normal, float3 viewTan : Tangent, float3 viewBitan : Bitangent, float2 tc : Texcoord) : SV_Target
{
    // 메쉬의 노말 값을 정규화
    viewNormal = normalize(viewNormal);
    // 노말맵을 적용할 경우 노말맵 텍스쳐를 샘플링 하여 노말값을 얻어옴.
    if (normalMapEnabled)
    {
        viewNormal = MapNormal(normalize(viewTan), normalize(viewBitan), viewNormal, tc, nmap, splr);
    }
    
    // 표면을 나타내는 fragment에서 광원을 향하는 벡터
    const float3 viewFragToL = viewLightPos - viewPos;
    const float distFragToL = length(viewFragToL);
    const float3 viewDirFragToL = viewFragToL / distFragToL;
    // 감쇠 값
    const float att = Attenuate(attConst, attLin, attQuad, distFragToL);
    // 난반사광
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, viewDirFragToL, viewNormal);
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
    // 반사된 specular 
    const float3 specularReflected = Speculate(
        specularReflectionColor, 1.0f, viewNormal,
        viewFragToL, viewPos, att, specularPower
    );
    // 난반사 텍스쳐 색상을 통해 감쇠된 난반사, 주변광 색상에 반사된 정반사광을 더해줌.
    return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specularReflected), 1.0f);
}