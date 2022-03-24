/*
	SV_PrimitiveID를 통해 각 면이 단일 색상을 갖도록 하는 셰이더.
*/

cbuffer CBUF
{
	float4 face_colors[8];
};

float4 main( uint tid : SV_PrimitiveID ) : SV_TARGET
{
	return face_colors[(tid / 2) % 8];
}