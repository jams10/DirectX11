/*
	래스터라이저를 통한 정점 보간을 통해 자동으로 블렌딩되는 색상을 갖게 하는 셰이더.
*/

cbuffer CBuf
{
	matrix transform;
};

struct VSOut
{
	float4 color : Color;
	float4 pos : SV_Position;
};

VSOut main(float3 pos : Position, float4 color : Color)
{
	VSOut vso;
	vso.pos = mul(float4(pos, 1.0f), transform);
	vso.color = color;
	return vso;
}