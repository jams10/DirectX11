float4 main( float2 pos : POSITION ) : SV_POSITION // �����ϴ� ������ ��ġ�� �ݵ�� 4���� �̾�� ��.
										           // (���� ��ǥ : homogeneous coordinate. ������ �� w�� �̿�, �Ŀ� �����Ͷ����� �ܰ迡�� perspective division�� �����ϱ� ����.)
{
	return float4(pos.x, pos.y, 0.0f, 1.0f);       // �׳� 2D �ﰢ���� ����� ���̱� ������ perspective division�� ���� �ʱ� ���� �׳� w���� 1�� ��.
}