#include "Box.h"
#include "../Bindable/BindableHeaders.h"
#include "../ErrorHandling/GraphicsThrowMacros.h"
#include "Primitive\Cube.h"

Box::Box(Graphics& gfx,
	std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist,
	std::uniform_real_distribution<float>& bdist,
	DirectX::XMFLOAT3 material)
	:
	TestObject(gfx, rng, adist, ddist, odist, rdist)
{
	if (!IsStaticInitialized())
	{
		struct Vertex
		{
			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT3 n;
		};

		// 정점 버퍼
		auto model = Cube::MakeIndependent<Vertex>();
		model.SetNormalsIndependentFlat(); // 노말 벡터 추가.

		AddStaticBind(std::make_unique<VertexBuffer>(gfx, model.vertices));

		// 정점 셰이더
		auto pvs = std::make_unique<VertexShader>(gfx, L"Shader\\PhongVS.cso");
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind(std::move(pvs));

		// 픽셀 셰이더
		AddStaticBind(std::make_unique<PixelShader>(gfx, L"Shader\\PhongPS.cso"));

		// 인덱스 버퍼
		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.indices));	

		// 입력 레이아웃
		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};
		AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));

		// 기본 도형
		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		SetIndexFromStatic();
	}
	
	// 정점 상수 버퍼
	AddBind(std::make_unique<TransformCbuf>(gfx, *this));

	// material 색상을 담은 상수 버퍼를 픽셀 셰이더에 바인딩.
	struct PSMaterialConstant
	{
		alignas(16) DirectX::XMFLOAT3 color;
		float specularIntensity = 0.6f;
		float specularPower = 30.0f;
		float padding[2];
	} colorConst;
	colorConst.color = material;
	AddBind(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, colorConst, 1u));

	// 각 인스턴스 당 달리 갖게 될 스케일 변환값.
	DirectX::XMStoreFloat3x3(
		&mt,
		DirectX::XMMatrixScaling(1.0f, 1.0f, bdist(rng))
	);
}

DirectX::XMMATRIX Box::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat3x3(&mt) * TestObject::GetTransformXM();
}