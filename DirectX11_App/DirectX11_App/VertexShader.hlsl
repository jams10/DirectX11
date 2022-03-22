float4 main( float2 pos : POSITION ) : SV_POSITION // 리턴하는 정점의 위치는 반드시 4차원 이어야 함.
										           // (동차 좌표 : homogeneous coordinate. 마지막 값 w를 이용, 후에 래스터라이저 단계에서 perspective division을 수행하기 위함.)
{
	return float4(pos.x, pos.y, 0.0f, 1.0f);       // 그냥 2D 삼각형을 찍어줄 것이기 때문에 perspective division을 하지 않기 위해 그냥 w값을 1로 줌.
}