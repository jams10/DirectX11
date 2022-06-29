#pragma once
#include <array>
#include "BindableCommon.h"
#include "../Graphics.h"
#include "Job.h"
#include "Pass.h"
#include "PerfLog.h"
#include "DepthStencil.h"
#include "RenderTarget.h"
#include <array>

class FrameCommander
{
public:
	FrameCommander(Graphics& gfx)
		:
		ds(gfx, gfx.GetWidth(), gfx.GetHeight()),
		rt(gfx, gfx.GetWidth(), gfx.GetHeight())
	{
		namespace dx = DirectX;

		// setup fullscreen geometry
		TemplateVertex::VertexLayout lay;
		lay.Append(TemplateVertex::VertexLayout::Position2D);
		TemplateVertex::VertexBuffer bufFull{ lay };
		bufFull.EmplaceBack(dx::XMFLOAT2{ -1,1 });
		bufFull.EmplaceBack(dx::XMFLOAT2{ 1,1 });
		bufFull.EmplaceBack(dx::XMFLOAT2{ -1,-1 });
		bufFull.EmplaceBack(dx::XMFLOAT2{ 1,-1 });
		pVbFull = Bind::VertexBuffer::Resolve(gfx, "$Full", std::move(bufFull));
		std::vector<unsigned short> indices = { 0,1,2,1,3,2 };
		pIbFull = Bind::IndexBuffer::Resolve(gfx, "$Full", std::move(indices));

		// setup fullscreen shaders
		pPsFull = Bind::PixelShader::Resolve(gfx, "Shader\\BlurOutline_PS.cso");
		pVsFull = Bind::VertexShader::Resolve(gfx, "Shader\\Fullscreen_VS.cso");
		pLayoutFull = Bind::InputLayout::Resolve(gfx, lay, pVsFull->GetBytecode());
		pSamplerFull = Bind::Sampler::Resolve(gfx, false, true);
		pBlenderFull = Bind::Blender::Resolve(gfx, true);
	}
	void Accept(Job job, size_t target) noexcept
	{
		passes[target].Accept(job);
	}
	void Execute(Graphics& gfx) const noxnd
	{
		using namespace Bind;
		// normally this would be a loop with each pass defining it setup / etc.
		// and later on it would be a complex graph with parallel execution contingent
		// on input / output requirements

		// setup render target used for normal passes
		ds.Clear(gfx);
		rt.Clear(gfx);
		gfx.BindSwapBuffer(ds);

		// main phong lighting pass
		Blender::Resolve(gfx, false)->Bind(gfx);
		Stencil::Resolve(gfx, Stencil::Mode::Off)->Bind(gfx);
		passes[0].Execute(gfx);
		// outline masking pass
		Stencil::Resolve(gfx, Stencil::Mode::Write)->Bind(gfx);
		NullPixelShader::Resolve(gfx)->Bind(gfx);
		passes[1].Execute(gfx);
		// outline drawing pass
		rt.BindAsTarget(gfx);
		Stencil::Resolve(gfx, Stencil::Mode::Off)->Bind(gfx);
		passes[2].Execute(gfx);
		// fullscreen blur + blend pass
		gfx.BindSwapBuffer(ds);
		rt.BindAsTexture(gfx, 0);
		pVbFull->Bind(gfx);
		pIbFull->Bind(gfx);
		pVsFull->Bind(gfx);
		pPsFull->Bind(gfx);
		pLayoutFull->Bind(gfx);
		pSamplerFull->Bind(gfx);
		pBlenderFull->Bind(gfx);
		Stencil::Resolve(gfx, Stencil::Mode::Mask)->Bind(gfx);
		gfx.DrawIndexed(pIbFull->GetCount());
	}
	void Reset() noexcept
	{
		for (auto& p : passes)
		{
			p.Reset();
		}
	}
private:
	std::array<Pass, 3> passes;
	DepthStencil ds;
	RenderTarget rt;
	std::shared_ptr<Bind::VertexBuffer> pVbFull;
	std::shared_ptr<Bind::IndexBuffer> pIbFull;
	std::shared_ptr<Bind::VertexShader> pVsFull;
	std::shared_ptr<Bind::PixelShader> pPsFull;
	std::shared_ptr<Bind::InputLayout> pLayoutFull;
	std::shared_ptr<Bind::Sampler> pSamplerFull;
	std::shared_ptr<Bind::Blender> pBlenderFull;
};