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

public:
	std::vector<T> vertices;
	std::vector<unsigned short> indices;
};