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
    float padding[2];
};

Texture2D tex;

SamplerState splr;

float4 main(float3 worldPos : Position, float3 n : Normal, float2 tc : Texcoord) : SV_Target
{
	// fragment to light vector data
    const float3 vToL = lightPos - worldPos; // 카메라 좌표 기준, 도형에서 광원을 향하는 벡터.
    const float distToL = length(vToL);
    const float3 dirToL = vToL / distToL;
	// diffuse 감쇠 값
    const float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));
	// diffuse intensity
    const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToL, n));
    
    // 반사광 벡터
    const float3 w = n * dot(vToL, n);
    const float3 r = w * 2.0f - vToL;
    // 시야 벡터와 반사 벡터 사이의 각도를 기준으로 반사광 강도를 계산함. specularPower 지수 값을 통해 반사광 효과 범위를 조절.
    const float3 specular = att * (diffuseColor * diffuseIntensity) * specularIntensity * pow(max(0.0f, dot(normalize(r), normalize(worldPos))), specularPower);
    
	// final color
    return float4(saturate(diffuse + ambient + specular), 1.0f) * tex.Sample(splr, tc);
}