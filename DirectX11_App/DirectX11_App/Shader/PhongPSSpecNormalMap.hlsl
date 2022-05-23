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
    // ���� �������� Ÿ�� ����(tan/bitan/normal)���� ��ȯ���ִ� ��� ����.
    const float3x3 tanToTarget = float3x3(
        normalize(tan),
        normalize(bitan),
        normalize(normal)
    );
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

float4 main(float3 viewPos : Position, float3 viewNormal : Normal, float3 viewTan : Tangent, float3 viewBitan : Bitangent, float2 tc : Texcoord) : SV_Target
{
    // �븻���� ������ ��� �븻�� �ؽ��ĸ� ���ø� �Ͽ� �븻���� ����.
    if (normalMapEnabled)
    {
        viewNormal = MapNormal(viewTan, viewBitan, viewNormal, tc, nmap, splr);
    }
    
    // ǥ���� ��Ÿ���� fragment���� ������ ���ϴ� ����
    const float3 viewFragToL = viewLightPos - viewPos;
    const float distFragToL = length(viewFragToL);
    const float3 viewDirFragToL = viewFragToL / distFragToL;
    // ���� ��
    const float att = Attenuate(attConst, attLin, attQuad, distFragToL);
    // ���ݻ籤
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, viewDirFragToL, viewNormal);
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
    // �ݻ�� specular 
    const float3 specularReflected = Speculate(
        specularReflectionColor, 1.0f, viewNormal,
        viewFragToL, viewPos, att, specularPower
    );
    // ���ݻ� �ؽ��� ������ ���� ����� ���ݻ�, �ֺ��� ���� �ݻ�� ���ݻ籤�� ������.
    return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specularReflected), 1.0f);
}