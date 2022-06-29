#pragma once
#include "BindableCommon.h"
#include "CustomMath.h"
#include "imgui.h"

class BlurPack
{
public:	
	BlurPack(Graphics& gfx, int radius = 7, float sigma = 2.6f)
		:
		shader(gfx, "Shader\\Blur_PS.cso"),
		pcb(gfx, 0u),
		ccb(gfx, 1u),
		radius(radius),
		sigma(sigma)
	{
		SetKernel(gfx, radius, sigma);
	}
	void Bind(Graphics& gfx) noexcept
	{
		shader.Bind(gfx);
		pcb.Bind(gfx);
		ccb.Bind(gfx);
	}
	void SetHorizontal(Graphics& gfx)
	{
		ccb.Update(gfx, { TRUE });
	}
	void SetVertical(Graphics& gfx)
	{
		ccb.Update(gfx, { FALSE });
	}
	void ShowWindow(Graphics& gfx)
	{
		ImGui::Begin("Blur");
		bool radChange = ImGui::SliderInt("Radius", &radius, 0, 15);
		bool sigChange = ImGui::SliderFloat("Sigma", &sigma, 0.1f, 10.0f);
		if (radChange || sigChange)
		{
			SetKernel(gfx, radius, sigma);
		}
		ImGui::End();
	}
	// nTaps should be 6sigma - 1
	// for more accurate coefs, need to integrate, but meh :/
	void SetKernel(Graphics& gfx, int radius, float sigma) noxnd
	{
		assert(radius <= 15);
		Kernel k;
		k.nTaps = radius * 2 + 1;
		float sum = 0.0f;
		for (int i = 0; i < k.nTaps; i++)
		{
			const auto x = float(i - radius);
			const auto g = gauss(x, sigma);
			sum += g;
			k.coefficients[i].x = g;
		}
		for (int i = 0; i < k.nTaps; i++)
		{
			k.coefficients[i].x /= sum; // coefficient의 값의 합을 1로 만들어주기 위해 정규화.
		}
		pcb.Update(gfx, k);
	}
private:
	int radius;
	float sigma;
	struct Kernel
	{
		int nTaps; // 커널의 너비.
		float padding[3];
		DirectX::XMFLOAT4 coefficients[31]; // 정렬로 인해 float4를 사용함.
	};
	struct Control
	{
		BOOL horizontal; // 셰이더에게 수평으로 처리하는 지 수직으로 처리하는 지 알려줌.
		float padding[3];
	};
	Bind::PixelShader shader;
	Bind::PixelConstantBuffer<Kernel> pcb;  // 커널을 위한 상수 버퍼
	Bind::PixelConstantBuffer<Control> ccb; // 컨트롤을 위한 상수 버퍼
};