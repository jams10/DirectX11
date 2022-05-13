#pragma once
#include "Vertex.h"
#include <vector>
#include <DirectXMath.h>

// IndexedTriangleList : TriangleList 기본 도형을 구성하는 정점과 인덱스들을 들고 있는 템플릿 클래스.
class IndexedTriangleList
{
public:
	IndexedTriangleList() = default;
	IndexedTriangleList(TemplateVertex::VertexBuffer verts_in, std::vector<unsigned short> indices_in) // 정점의 경우 어떻게 정의하느냐에 따라 다르기 때문에 템플릿 타입으로 받아줌.
		:
		vertices(std::move(verts_in)),
		indices(std::move(indices_in))
	{
		assert(vertices.Size() > 2);     // 삼각형이므로, 정점의 개수가 최소 3 이상이어야 함.
		assert(indices.size() % 3 == 0); // 삼각형 하나를 구성하는 인덱스는 3개이므로, 인덱스 개수가 3의 배수이어야 함.
	}
	// 각 정점의 위치에 대해 변환 행렬을 적용하는 함수.
	void Transform(DirectX::FXMMATRIX matrix)
	{
		using Elements = TemplateVertex::VertexLayout::ElementType;
		for (int i = 0; i < vertices.Size(); i++)
		{
			auto& pos = vertices[i].Attr<Elements::Position3D>();     // 세 개의 float 값인 pos를 벡터 형태로 변환.
			DirectX::XMStoreFloat3(									  // 벡터 형태로 변환했으므로, 변환 행렬을 통해 변환을 수행하고, 다시 float 세 개의 값으로 바꿔줌.
				&pos,
				DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&pos), matrix)
			);
		}
	}
	// 물체의 각 표면을 구성하는 정점들에 노말 벡터를 추가해주는 함수. 
	void SetNormalsIndependentFlat() noxnd
	{
		 using namespace DirectX;
		 using Type = TemplateVertex::VertexLayout::ElementType;
		 for (size_t i = 0; i < indices.size(); i += 3)
		 {
			 auto v0 = vertices[indices[i]];
			 auto v1 = vertices[indices[i + 1]];
			 auto v2 = vertices[indices[i + 2]];
			 const auto p0 = XMLoadFloat3(&v0.Attr<Type::Position3D>());
			 const auto p1 = XMLoadFloat3(&v1.Attr<Type::Position3D>());
			 const auto p2 = XMLoadFloat3(&v2.Attr<Type::Position3D>());
			 const auto n = XMVector3Normalize(XMVector3Cross((p1 - p0), (p2 - p0)));

			 XMStoreFloat3(&v0.Attr<Type::Normal>(), n);
			 XMStoreFloat3(&v1.Attr<Type::Normal>(), n);
			 XMStoreFloat3(&v2.Attr<Type::Normal>(), n);
		 }
	}

public:
	TemplateVertex::VertexBuffer vertices;
	std::vector<unsigned short> indices;
};