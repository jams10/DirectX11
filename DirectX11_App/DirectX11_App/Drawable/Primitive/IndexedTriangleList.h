#pragma once
#include <vector>
#include <DirectXMath.h>

// IndexedTriangleList : TriangleList �⺻ ������ �����ϴ� ������ �ε������� ��� �ִ� ���ø� Ŭ����.
template<class T>
class IndexedTriangleList
{
public:
	IndexedTriangleList() = default;
	IndexedTriangleList(std::vector<T> verts_in, std::vector<unsigned short> indices_in) // ������ ��� ��� �����ϴ��Ŀ� ���� �ٸ��� ������ ���ø� Ÿ������ �޾���.
		:
		vertices(std::move(verts_in)),
		indices(std::move(indices_in))
	{
		assert(vertices.size() > 2);     // �ﰢ���̹Ƿ�, ������ ������ �ּ� 3 �̻��̾�� ��.
		assert(indices.size() % 3 == 0); // �ﰢ�� �ϳ��� �����ϴ� �ε����� 3���̹Ƿ�, �ε��� ������ 3�� ����̾�� ��.
	}
	// �� ������ ��ġ�� ���� ��ȯ ����� �����ϴ� �Լ�.
	void Transform(DirectX::FXMMATRIX matrix)
	{
		for (auto& v : vertices)
		{
			const DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&v.pos); // �� ���� float ���� pos�� ���� ���·� ��ȯ.
			DirectX::XMStoreFloat3(									     // ���� ���·� ��ȯ�����Ƿ�, ��ȯ ����� ���� ��ȯ�� �����ϰ�, �ٽ� float �� ���� ������ �ٲ���.
				&v.pos,
				DirectX::XMVector3Transform(pos, matrix)
			);
		}
	}
	// ��ü�� �� ǥ���� �����ϴ� �����鿡 �븻 ���͸� �߰����ִ� �Լ�. 
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