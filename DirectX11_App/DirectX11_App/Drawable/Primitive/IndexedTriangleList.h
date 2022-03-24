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

public:
	std::vector<T> vertices;
	std::vector<unsigned short> indices;
};