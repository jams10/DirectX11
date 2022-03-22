struct VSOUT
{
	float4 color : COLOR;
	float4 pos : SV_POSITION;
};

cbuffer CBUF
{
	row_major matrix transform; // matrix 키워드는 4 * 4 자료형을 의미.
};

VSOUT main(float2 pos : POSITION, float4 color : COLOR)
{
	VSOUT vso;
	vso.pos = mul( float4(pos.x, pos.y, 0.0f, 1.0f), transform);
	vso.color = color;
	return vso;
}