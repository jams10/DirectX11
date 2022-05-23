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