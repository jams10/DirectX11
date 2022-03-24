#pragma once
#include "IndexedTriangleList.h"
#include <DirectXMath.h>
#include "CustomMath.h"

// Cone : IndexedTriangleList 클래스를 이용, 각뿔을 위한 정점과 인덱스를 구성해주는 클래스.
class Cone
{
public:
	// 각뿔을 위한 정점과 인덱스를 구성해주는 템플릿 함수. longDiv : 각뿔의 밑면을 구성할 정점의 개수.
	template<class V>
	static IndexedTriangleList<V> MakeTesselated(int longDiv)
	{
		assert(longDiv >= 3); // 각뿔의 기본은 삼각뿔이므로, longDiv 값이 3 이상이어야 함.

		const auto base = DirectX::XMVectorSet(1.0f, 0.0f, -1.0f, 0.0f);
		const float longitudeAngle = 2.0f * PI / longDiv;

		// 밑면을 구성할 정점들.
		std::vector<V> vertices;
		for (int iLong = 0; iLong < longDiv; iLong++)
		{
			vertices.emplace_back();
			auto v = DirectX::XMVector3Transform(
				base,
				DirectX::XMMatrixRotationZ(longitudeAngle * iLong)
			);
			DirectX::XMStoreFloat3(&vertices.back().pos, v);
		}

		// 밑면의 중심에 위치한 정점.
		vertices.emplace_back();
		vertices.back().pos = { 0.0f,0.0f,-1.0f };
		const auto iCenter = (unsigned short)(vertices.size() - 1); // 밑면을 위한 인덱스를 만들어주기 위해 따로 저장해둠.
		// 뿔의 꼭짓점을 위한 정점.
		vertices.emplace_back();
		vertices.back().pos = { 0.0f,0.0f,1.0f };
		const auto iTip = (unsigned short)(vertices.size() - 1);    // 옆면을 위한 인덱스를 만들어주기 위해 따로 저장해둠.

		// 밑면 인덱스들
		std::vector<unsigned short> indices;
		for (unsigned short iLong = 0; iLong < longDiv; iLong++)
		{
			indices.push_back(iCenter);
			indices.push_back((iLong + 1) % longDiv);
			indices.push_back(iLong);
		}

		// 옆면 인덱스들
		for (unsigned short iLong = 0; iLong < longDiv; iLong++)
		{
			indices.push_back(iLong);
			indices.push_back((iLong + 1) % longDiv);
			indices.push_back(iTip);
		}

		return { std::move(vertices),std::move(indices) };
	}
	template<class V>
	static IndexedTriangleList<V> Make()
	{
		return MakeTesselated<V>(24);
	}
};