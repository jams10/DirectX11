cbuffer LightCBuf
{
    float3 lightPos;
    float3 ambient;
    float3 diffuseColor;
    float diffuseIntensity;
    float attConst;
    float attLin;
    float attQuad;
};

cbuffer ObjectCBuf
{
    float specularIntensity;
    float specularPower;
    bool normalMapEnabled; // 노말맵 적용 여부
    float padding[1];
};

Texture2D tex;                 // 디퓨즈맵
Texture2D nmap : register(t2); // 노말맵 

SamplerState splr;


float4 main(float3 viewPos : Position, float3 viewNormal : Normal, float3 tan : Tangent, float3 bitan : Bitangent, float2 tc : Texcoord) : SV_Target
{
    // 노말맵을 적용할 경우 노말맵 텍스쳐를 샘플링 하여 노말값을 얻어옴.
    if (normalMapEnabled)
    {
        // 접선 공간에서 뷰 공간으로 변환하기 위한 행렬 생성.
        const float3x3 tanToView = float3x3(
            normalize(tan),
            normalize(bitan),
            normalize(viewNormal)
        );
        // 노말 맵으로 부터 노말 값 얻어오기. 해당 노말 값은 탄젠트 공간임.
        const float3 normalSample = nmap.Sample(splr, tc).xyz;
        float3 tanNormal;
        tanNormal = normalSample * 2.0f - 1.0f;
        tanNormal.y = -tanNormal.y;
        // 얻어온 노말 값을 뷰 공간으로 변환하고, 정규화 함.
        viewNormal = normalize(mul(tanNormal, tanToView));
    }
	// fragment to light vector data
    const float3 vToL = lightPos - viewPos;
    const float distToL = length(vToL);
    const float3 dirToL = vToL / distToL;
	// 감쇠 값(attenuation)
    const float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));
    // 디퓨즈 값
    const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToL, viewNormal));
    // 반사 벡터
    const float3 w = viewNormal * dot(vToL, viewNormal);
    const float3 r = w * 2.0f - vToL;
	// calculate specular intensity based on angle between viewing vector and reflection vector, narrow with power function
    const float3 specular = att * (diffuseColor * diffuseIntensity) * specularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(viewPos))), specularPower);
	// final color
    return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specular), 1.0f);
}