#pragma once
#include "IndexedTriangleList.h"
#include <DirectXMath.h>
#include "CustomMath.h"

// Prism : IndexedTriangleList�� �̿�, ������� ���� ������ �ε����� �������ִ� Ŭ����.
class Prism
{
public:
	// ������� �׸��� ���� ������ �ε����� �������ִ� ���ø� �Լ�. longDiv : �ظ��� ������ ������ ����.
	template<class V>
	static IndexedTriangleList<V> MakeTesselated(int longDiv)
	{
		assert(longDiv >= 3);

		const auto base = DirectX::XMVectorSet(1.0f, 0.0f, -1.0f, 0.0f);  // �ظ�.
		const auto offset = DirectX::XMVectorSet(0.0f, 0.0f, 2.0f, 0.0f); // far base�� ���� z�� ������.
		const float longitudeAngle = 2.0f * PI / longDiv;

		// near center
		std::vector<V> vertices;
		vertices.emplace_back();
		vertices.back().pos = { 0.0f,0.0f,-1.0f };
		const auto iCenterNear = (unsigned short)(vertices.size() - 1);
		// far center
		vertices.emplace_back();
		vertices.back().pos = { 0.0f,0.0f,1.0f };
		const auto iCenterFar = (unsigned short)(vertices.size() - 1);

		// base vertices
		for (int iLong = 0; iLong < longDiv; iLong++)
		{
			// near base
			{
				vertices.emplace_back();
				auto v = DirectX::XMVector3Transform(
					base,
					DirectX::XMMatrixRotationZ(longitudeAngle * iLong)
				);
				DirectX::XMStoreFloat3(&vertices.back().pos, v);
			}
			// far base
			{
				vertices.emplace_back();
				auto v = DirectX::XMVector3Transform(
					base,
					DirectX::XMMatrixRotationZ(longitudeAngle * iLong)
				);
				v = dx::XMVectorAdd(v, offset); // far base �̹Ƿ� offset�� ������.
				DirectX::XMStoreFloat3(&vertices.back().pos, v);
			}
		}

		// side indices
		std::vector<unsigned short> indices;
		for (unsigned short iLong = 0; iLong < longDiv; iLong++)
		{
			const auto i = iLong * 2;
			const auto mod = longDiv * 2;
			indices.push_back(i + 2);
			indices.push_back((i + 2) % mod + 2);
			indices.push_back(i + 1 + 2);
			indices.push_back((i + 2) % mod + 2);
			indices.push_back((i + 3) % mod + 2);
			indices.push_back(i + 1 + 2);
		}

		// base indices
		for (unsigned short iLong = 0; iLong < longDiv; iLong++)
		{
			const auto i = iLong * 2;
			const auto mod = longDiv * 2;
			indices.push_back(i + 2);
			indices.push_back(iCenterNear);
			indices.push_back((i + 2) % mod + 2);
			indices.push_back(iCenterFar);
			indices.push_back(i + 1 + 2);
			indices.push_back((i + 3) % mod + 2);
		}

		return { std::move(vertices),std::move(indices) };
	}
	template<class V>
	static IndexedTriangleList<V> Make()
	{
		return MakeTesselated<V>(24);
	}
};