#include "TransformCBufVtxPxl.h"

namespace Bind
{
	std::unique_ptr<PixelConstantBuffer<TransformCbuf::Transforms>> TransformCBufVtxPxl::pPcbuf;

	TransformCBufVtxPxl::TransformCBufVtxPxl(Graphics& gfx, const Drawable& parent, UINT slotV, UINT slotP)
		:
		TransformCbuf(gfx, parent, slotV)
	{
		if (!pPcbuf)
		{
			pPcbuf = std::make_unique<PixelConstantBuffer<Transforms>>(gfx, slotP);
		}
	}

	void Bind::TransformCBufVtxPxl::Bind(Graphics& gfx) noexcept
	{
		const auto tf = GetTransforms(gfx);
		TransformCbuf::UpdateBindImpl(gfx, tf);
		UpdateBindImpl(gfx, tf);
	}

	void TransformCBufVtxPxl::UpdateBindImpl(Graphics& gfx, const Transforms& tf) noexcept
	{
		pPcbuf->Update(gfx, tf);
		pPcbuf->Bind(gfx);
	}
}