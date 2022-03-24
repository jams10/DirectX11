#pragma once
#include "IndexedTriangleList.h"
#include <DirectXMath.h>

// Cube : IndexedTriangleList를 이용, 정육면체를 위한 정점과 인덱스를 구성해주는 클래스.
class Cube
{
public:
	// 정육면체를 구성하는 기본적인 8개의 정점, 이에 따른 인덱스를 만들어주는 템플릿 함수.
	template<class V> // 역시 정점 구조가 여러가지이므로 템플릿으로 받아줌.
	static IndexedTriangleList<V> Make()
	{
		constexpr float side = 1.0f / 2.0f;

		// 정점을 생성하고 위치를 받아줌.
		std::vector<V> vertices(8);
		vertices[0].pos = { -side,-side,-side };
		vertices[1].pos = { side,-side,-side };
		vertices[2].pos = { -side,side,-side };
		vertices[3].pos = { side,side,-side };
		vertices[4].pos = { -side,-side,side };
		vertices[5].pos = { side,-side,side };
		vertices[6].pos = { -side,side,side };
		vertices[7].pos = { side,side,side };

		// 정점과 인덱스를 담은 IndexedTriangleList를 리턴해줌.
		return{
			std::move(vertices),{
				0,2,1, 2,3,1,
				1,3,5, 3,7,5,
				2,6,3, 3,6,7,
				4,5,7, 4,7,6,
				0,4,2, 2,4,6,
				0,1,4, 1,5,4
			}
		};
	}

	template<class V>
	static IndexedTriangleList<V> MakeSkinned()
	{
		namespace dx = DirectX;

		constexpr float side = 1.0f / 2.0f;

		std::vector<V> vertices(14);

		vertices[0].pos = { -side,-side,-side };
		vertices[0].tex = { 2.0f / 3.0f,0.0f / 4.0f };
		vertices[1].pos = { side,-side,-side };
		vertices[1].tex = { 1.0f / 3.0f,0.0f / 4.0f };
		vertices[2].pos = { -side,side,-side };
		vertices[2].tex = { 2.0f / 3.0f,1.0f / 4.0f };
		vertices[3].pos = { side,side,-side };
		vertices[3].tex = { 1.0f / 3.0f,1.0f / 4.0f };
		vertices[4].pos = { -side,-side,side };
		vertices[4].tex = { 2.0f / 3.0f,3.0f / 4.0f };
		vertices[5].pos = { side,-side,side };
		vertices[5].tex = { 1.0f / 3.0f,3.0f / 4.0f };
		vertices[6].pos = { -side,side,side };
		vertices[6].tex = { 2.0f / 3.0f,2.0f / 4.0f };
		vertices[7].pos = { side,side,side };
		vertices[7].tex = { 1.0f / 3.0f,2.0f / 4.0f };
		vertices[8].pos = { -side,-side,-side };
		vertices[8].tex = { 2.0f / 3.0f,4.0f / 4.0f };
		vertices[9].pos = { side,-side,-side };
		vertices[9].tex = { 1.0f / 3.0f,4.0f / 4.0f };
		vertices[10].pos = { -side,-side,-side };
		vertices[10].tex = { 3.0f / 3.0f,1.0f / 4.0f };
		vertices[11].pos = { -side,-side,side };
		vertices[11].tex = { 3.0f / 3.0f,2.0f / 4.0f };
		vertices[12].pos = { side,-side,-side };
		vertices[12].tex = { 0.0f / 3.0f,1.0f / 4.0f };
		vertices[13].pos = { side,-side,side };
		vertices[13].tex = { 0.0f / 3.0f,2.0f / 4.0f };

		return{
			std::move(vertices),{
				0,2,1,   2,3,1,
				4,8,5,   5,8,9,
				2,6,3,   3,6,7,
				4,5,7,   4,7,6,
				2,10,11, 2,11,6,
				12,3,7,  12,7,13
			}
		};
	}
};