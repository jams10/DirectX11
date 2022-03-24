#pragma once
#include "IndexedTriangleList.h"
#include <DirectXMath.h>
#include "CustomMath.h"

// Cone : IndexedTriangleList Ŭ������ �̿�, ������ ���� ������ �ε����� �������ִ� Ŭ����.
class Cone
{
public:
	// ������ ���� ������ �ε����� �������ִ� ���ø� �Լ�. longDiv : ������ �ظ��� ������ ������ ����.
	template<class V>
	static IndexedTriangleList<V> MakeTesselated(int longDiv)
	{
		assert(longDiv >= 3); // ������ �⺻�� �ﰢ���̹Ƿ�, longDiv ���� 3 �̻��̾�� ��.

		const auto base = DirectX::XMVectorSet(1.0f, 0.0f, -1.0f, 0.0f);
		const float longitudeAngle = 2.0f * PI / longDiv;

		// �ظ��� ������ ������.
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

		// �ظ��� �߽ɿ� ��ġ�� ����.
		vertices.emplace_back();
		vertices.back().pos = { 0.0f,0.0f,-1.0f };
		const auto iCenter = (unsigned short)(vertices.size() - 1); // �ظ��� ���� �ε����� ������ֱ� ���� ���� �����ص�.
		// ���� �������� ���� ����.
		vertices.emplace_back();
		vertices.back().pos = { 0.0f,0.0f,1.0f };
		const auto iTip = (unsigned short)(vertices.size() - 1);    // ������ ���� �ε����� ������ֱ� ���� ���� �����ص�.

		// �ظ� �ε�����
		std::vector<unsigned short> indices;
		for (unsigned short iLong = 0; iLong < longDiv; iLong++)
		{
			indices.push_back(iCenter);
			indices.push_back((iLong + 1) % longDiv);
			indices.push_back(iLong);
		}

		// ���� �ε�����
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