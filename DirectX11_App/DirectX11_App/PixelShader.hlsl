cbuffer CBUF
{
	float4 face_colors[6];
};

float4 main( uint tid : SV_PrimitiveID ) : SV_TARGET
{
	return face_colors[tid/2]; // �簢�� ���� �� ���� �ﰢ���� �����Ƿ�, �� ���� ���� ������ ������ tid / 2 �ε����� ����.
}