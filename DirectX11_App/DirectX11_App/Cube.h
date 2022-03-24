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

		// 정점의 위치 생성.
		std::vector<DirectX::XMFLOAT3> vertices;
		vertices.emplace_back(-side, -side, -side); // 0
		vertices.emplace_back(side, -side, -side);  // 1
		vertices.emplace_back(-side, side, -side);  // 2
		vertices.emplace_back(side, side, -side);   // 3
		vertices.emplace_back(-side, -side, side);  // 4
		vertices.emplace_back(side, -side, side);   // 5
		vertices.emplace_back(-side, side, side);   // 6
		vertices.emplace_back(side, side, side);    // 7

		// 정점을 생성하고 정점의 위치를 담아줌.
		std::vector<V> verts(vertices.size());
		for (size_t i = 0; i < vertices.size(); i++)
		{
			verts[i].pos = vertices[i];
		}

		// 정점과 인덱스를 담은 IndexedTriangleList를 리턴해줌.
		return{
			std::move(verts),{
				0,2,1, 2,3,1,
				1,3,5, 3,7,5,
				2,6,3, 3,6,7,
				4,5,7, 4,7,6,
				0,4,2, 2,4,6,
				0,1,4, 1,5,4
			}
		};
	}
};