cbuffer CBUF
{
	matrix transform; // matrix 키워드는 4 * 4 자료형을 의미.
};

float4 main(float3 pos : POSITION) : SV_POSITION
{
	return mul(float4(pos, 1.0f), transform);
}