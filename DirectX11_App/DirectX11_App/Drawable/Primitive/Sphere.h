#pragma once
#include <optional>
#include "Vertex.h"
#include "IndexedTriangleList.h"
#include <DirectXMath.h>
#include "CustomMath.h"

// Sphere : IndexedTriangleList 클래스를 이용, 구체를 위한 정점과 인덱스를 구성해주는 클래스.
class Sphere
{
public:
	// 구체를 위한 정점과 인덱스를 구성해주는 템플릿 클래스. latDiv : 위도(y축) 분할 정점 개수 / longDiv : 경도(x축) 분할 정점 개수.
	static IndexedTriangleList MakeTesselated(TemplateVertex::VertexLayout layout, int latDiv, int longDiv)
	{
		assert(latDiv >= 3);
		assert(longDiv >= 3);

		constexpr float radius = 1.0f;
		const auto base = DirectX::XMVectorSet(0.0f, 0.0f, radius, 0.0f);
		const float lattitudeAngle = PI / latDiv;
		const float longitudeAngle = 2.0f * PI / longDiv;

		TemplateVertex::VertexBuffer vb{ std::move(layout) };
		for (int iLat = 1; iLat < latDiv; iLat++)
		{
			const auto latBase = DirectX::XMVector3Transform(
				base,
				DirectX::XMMatrixRotationX(lattitudeAngle * iLat)
			);
			for (int iLong = 0; iLong < longDiv; iLong++)
			{
				DirectX::XMFLOAT3 calculatedPos;
				auto v = DirectX::XMVector3Transform(
					latBase,
					DirectX::XMMatrixRotationZ(longitudeAngle * iLong)
				);
				DirectX::XMStoreFloat3(&calculatedPos, v);
				vb.EmplaceBack(calculatedPos);
			}
		}

		// 위 뚜껑을 덮어주기 위한 정점들을 만들어줌.
		const auto iNorthPole = (unsigned short)vb.Size();
		{
			DirectX::XMFLOAT3 northPos;
			DirectX::XMStoreFloat3(&northPos, base);
			vb.EmplaceBack(northPos);
		}
		const auto iSouthPole = (unsigned short)vb.Size();
		{
			DirectX::XMFLOAT3 southPos;
			DirectX::XMStoreFloat3(&southPos, DirectX::XMVectorNegate(base));
			vb.EmplaceBack(southPos);
		}

		const auto calcIdx = [latDiv, longDiv](unsigned short iLat, unsigned short iLong) // 인덱스 계산을 위한 람다 함수.
		{ return iLat * longDiv + iLong; };

		std::vector<unsigned short> indices;
		for (unsigned short iLat = 0; iLat < latDiv - 2; iLat++)
		{
			for (unsigned short iLong = 0; iLong < longDiv - 1; iLong++)
			{
				indices.push_back(calcIdx(iLat, iLong));
				indices.push_back(calcIdx(iLat + 1, iLong));
				indices.push_back(calcIdx(iLat, iLong + 1));
				indices.push_back(calcIdx(iLat, iLong + 1));
				indices.push_back(calcIdx(iLat + 1, iLong));
				indices.push_back(calcIdx(iLat + 1, iLong + 1));
			}
			// wrap band
			indices.push_back(calcIdx(iLat, longDiv - 1));
			indices.push_back(calcIdx(iLat + 1, longDiv - 1));
			indices.push_back(calcIdx(iLat, 0));
			indices.push_back(calcIdx(iLat, 0));
			indices.push_back(calcIdx(iLat + 1, longDiv - 1));
			indices.push_back(calcIdx(iLat + 1, 0));
		}

		// cap fans
		for (unsigned short iLong = 0; iLong < longDiv - 1; iLong++)
		{
			// north
			indices.push_back(iNorthPole);
			indices.push_back(calcIdx(0, iLong));
			indices.push_back(calcIdx(0, iLong + 1));
			// south
			indices.push_back(calcIdx(latDiv - 2, iLong + 1));
			indices.push_back(calcIdx(latDiv - 2, iLong));
			indices.push_back(iSouthPole);
		}
		// wrap triangles
		// north
		indices.push_back(iNorthPole);
		indices.push_back(calcIdx(0, longDiv - 1));
		indices.push_back(calcIdx(0, 0));
		// south
		indices.push_back(calcIdx(latDiv - 2, 0));
		indices.push_back(calcIdx(latDiv - 2, longDiv - 1));
		indices.push_back(iSouthPole);

		return { std::move(vb),std::move(indices) };
	}
	static IndexedTriangleList Make(std::optional<TemplateVertex::VertexLayout> layout = std::nullopt)
	{
		using Element = TemplateVertex::VertexLayout::ElementType;
		if (!layout)
		{
			layout = TemplateVertex::VertexLayout{}.Append(Element::Position3D);
		}
		return MakeTesselated(std::move(*layout), 12, 24);
	}
};