#include "TransformCbuf.h"

std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> TransformCbuf::pVcbuf; // static 멤버의 경우 구현부에 정의.

TransformCbuf::TransformCbuf(Graphics& gfx, const Drawable& parent)
	:
	parent(parent)
{
	if (!pVcbuf)
	{
		pVcbuf = std::make_unique<VertexConstantBuffer<DirectX::XMMATRIX>>(gfx);
	}
}

void TransformCbuf::Bind(Graphics& gfx) noexcept
{
	pVcbuf->Update(gfx,
		DirectX::XMMatrixTranspose(
			parent.GetTransformXM() * gfx.GetProjection()
		)
	);
	pVcbuf->Bind(gfx);
}