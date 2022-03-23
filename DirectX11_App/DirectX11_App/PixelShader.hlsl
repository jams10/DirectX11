cbuffer CBUF
{
	float4 face_colors[6];
};

float4 main( uint tid : SV_PrimitiveID ) : SV_TARGET
{
	return face_colors[tid/2]; // 사각형 마다 두 개의 삼각형을 가지므로, 한 면이 단일 색상을 갖도록 tid / 2 인덱스로 접근.
}