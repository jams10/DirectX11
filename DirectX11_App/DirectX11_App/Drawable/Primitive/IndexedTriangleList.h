#pragma once
#include <vector>
#include <DirectXMath.h>

// IndexedTriangleList : TriangleList 기본 도형을 구성하는 정점과 인덱스들을 들고 있는 템플릿 클래스.
template<class T>
class IndexedTriangleList
{
public:
	IndexedTriangleList() = default;
	IndexedTriangleList(std::vector<T> verts_in, std::vector<unsigned short> indices_in) // 정점의 경우 어떻게 정의하느냐에 따라 다르기 때문에 템플릿 타입으로 받아줌.
		:
		vertices(std::move(verts_in)),
		indices(std::move(indices_in))
	{
		assert(vertices.size() > 2);     // 삼각형이므로, 정점의 개수가 최소 3 이상이어야 함.
		assert(indices.size() % 3 == 0); // 삼각형 하나를 구성하는 인덱스는 3개이므로, 인덱스 개수가 3의 배수이어야 함.
	}
	// 각 정점의 위치에 대해 변환 행렬을 적용하는 함수.
	void Transform(DirectX::FXMMATRIX matrix)
	{
		for (auto& v : vertices)
		{
			const DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&v.pos); // 세 개의 float 값인 pos를 벡터 형태로 변환.
			DirectX::XMStoreFloat3(									     // 벡터 형태로 변환했으므로, 변환 행렬을 통해 변환을 수행하고, 다시 float 세 개의 값으로 바꿔줌.
				&v.pos,
				DirectX::XMVector3Transform(pos, matrix)
			);
		}
	}
	// 물체의 각 표면을 구성하는 정점들에 노말 벡터를 추가해주는 함수. 
	void SetNormalsIndependentFlat() noxnd
	{
		using namespace DirectX;
		assert(indices.size() % 3 == 0 && indices.size() > 0);
		for (size_t i = 0; i < indices.size(); i += 3)
		{
			auto& v0 = vertices[indices[i]];
			auto& v1 = vertices[indices[i + 1]];
			auto& v2 = vertices[indices[i + 2]];
			const auto p0 = XMLoadFloat3(&v0.pos);
			const auto p1 = XMLoadFloat3(&v1.pos);
			const auto p2 = XMLoadFloat3(&v2.pos);

			const auto n = XMVector3Normalize(XMVector3Cross((p1 - p0), (p2 - p0)));

			XMStoreFloat3(&v0.n, n);
			XMStoreFloat3(&v1.n, n);
			XMStoreFloat3(&v2.n, n);
		}
	}

public:
	std::vector<T> vertices;
	std::vector<unsigned short> indices;
};