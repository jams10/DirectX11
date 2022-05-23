float3 MapNormal(
    const in float3 tan,
    const in float3 bitan,
    const in float3 normal,
    const in float2 tc,
    uniform Texture2D nmap,
    uniform SamplerState splr)
{
    // ���� �������� Ÿ�� ����(tan/bitan/normal)���� ��ȯ���ִ� ��� ����.
    const float3x3 tanToTarget = float3x3(tan, bitan, normal);
    // �븻 ������ ���� �븻 �� ���� �ڿ� �븻 �� ������ �´� ������ ��������.
    const float3 normalSample = nmap.Sample(splr, tc).xyz;
    const float3 tanNormal = normalSample * 2.0f - 1.0f;
    // ���� �븻 ���� Ÿ�� �������� ��ȯ.
    return normalize(mul(tanNormal, tanToTarget));
}

float Attenuate(uniform float attConst, uniform float attLin, uniform float attQuad, const in float distFragToL)
{
    return 1.0f / (attConst + attLin * distFragToL + attQuad * (distFragToL * distFragToL));
}

float3 Diffuse( // ���ݻ簪 ��� �Լ�.
    uniform float3 diffuseColor,
    uniform float diffuseIntensity,
    const in float att,
    const in float3 viewDirFragToL,
    const in float3 viewNormal)
{
    return diffuseColor * diffuseIntensity * att * max(0.0f, dot(viewDirFragToL, viewNormal));
}

float3 Speculate( // ���ݻ簪 ��� �Լ�.
    const in float3 specularColor,
    uniform float specularIntensity,
    const in float3 viewNormal,
    const in float3 viewFragToL,
    const in float3 viewPos,
    const in float att,
    const in float specularPower)
{
    // �ݻ籤 ���͸� ���.
    const float3 w = viewNormal * dot(viewFragToL, viewNormal);
    const float3 r = normalize(w * 2.0f - viewFragToL);
    // ī�޶󿡼� ��ü�� ǥ���� ��Ÿ���� fragment�� ���ϴ� ���� (�� ����)
    const float3 viewCamToFrag = normalize(viewPos);
    // �þ� ���Ϳ� �ݻ� ���� ������ ������ �������� �� specular ��� ���� ���� �����. ���� �Լ��� specular ������ ����.
    return att * specularColor * specularIntensity * pow(max(0.0f, dot(-r, viewCamToFrag)), specularPower);
}