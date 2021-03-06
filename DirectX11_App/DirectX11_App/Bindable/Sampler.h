#pragma once
#include "Bindable.h"

namespace Bind
{
	class Sampler : public Bindable
	{
	public:
		Sampler(Graphics& gfx, bool anisoEnable, bool reflect);
		void Bind(Graphics& gfx) noexcept override;
		static std::shared_ptr<Sampler> Resolve(Graphics& gfx, bool anisoEnable = true, bool reflect = false);
		static std::string GenerateUID(bool anisoEnable, bool reflect);
		std::string GetUID() const noexcept override;
	protected:
		// Sampler 혹은 SamplerState는 파이프라인에게 텍스쳐에 대한 Lookup(조회)을 어떻게 수행해야 하는지 알려줌.
		Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;
		bool anisoEnable;
		bool reflect;
	};
}