#include "App.h"
#include "Box.h"
#include "Pyramid.h"
#include "Melon.h"
#include "Sheet.h"
#include "SkinnedBox.h"
#include "CustomMath.h"
#include <memory>
#include "Surface.h"
#include "GDIPlusManager.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "StringEncoding.h"

GDIPlusManager gdipm; // GDI+ ���̺귯���� ����ϱ� ���� �ռ� �ʱ�ȭ ���־�� ��. ������ ȣ���� ���� �ʱ�ȭ�� ����.

App::App()
	:
	wnd(1080, 720, L"������!")
{
	class Factory
	{
	public:
		Factory(Graphics& gfx)
			:
			gfx(gfx)
		{}
		std::unique_ptr<Drawable> operator()()
		{
			switch (typedist(rng))
			{
			case 0:
				return std::make_unique<Pyramid>(
					gfx, rng, adist, ddist,
					odist, rdist
					);
			case 1:
				return std::make_unique<Box>(
					gfx, rng, adist, ddist,
					odist, rdist, bdist
					);
			case 2:
				return std::make_unique<Melon>(
					gfx, rng, adist, ddist,
					odist, rdist, longdist, latdist
					);
			case 3:
				return std::make_unique<Sheet>(
					gfx, rng, adist, ddist,
					odist, rdist
					);
			case 4:
				return std::make_unique<SkinnedBox>(
					gfx, rng, adist, ddist,
					odist, rdist
					);
			default:
				assert(false && "bad drawable type in factory");
				return {};
			}
		}
	private:
		Graphics& gfx;
		std::mt19937 rng{ std::random_device{}() };
		std::uniform_real_distribution<float> adist{ 0.0f,PI * 2.0f };
		std::uniform_real_distribution<float> ddist{ 0.0f,PI * 0.5f };
		std::uniform_real_distribution<float> odist{ 0.0f,PI * 0.08f };
		std::uniform_real_distribution<float> rdist{ 6.0f,20.0f };
		std::uniform_real_distribution<float> bdist{ 0.4f,3.0f };
		std::uniform_int_distribution<int> latdist{ 5,20 };
		std::uniform_int_distribution<int> longdist{ 10,40 };
		std::uniform_int_distribution<int> typedist{ 0,4 }; // ���� Ÿ�� ���� ���� ���� ����
	};

	drawables.reserve(nDrawables);
	std::generate_n(std::back_inserter(drawables), nDrawables, Factory{ wnd.Gfx() });

	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 720.0f / 1080.0f, 0.5f, 40.0f));
};

void App::DoFrame()
{
	gt.Tick();
	const auto dt = gt.GetDeltaTime() * speed_factor;

	if (wnd.kbd.KeyIsPressed(VK_SPACE))
	{
		wnd.Gfx().DisableImgui();
	}
	else
	{
		wnd.Gfx().EnableImgui();
	}

	wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);

	// ������ ������
	for (auto& d : drawables)
	{
		d->Update(wnd.kbd.KeyIsPressed(VK_SPACE) ? 0.0f : dt);
		d->Draw(wnd.Gfx());
	}

	static char buffer[1024];

	// �ùķ��̼� �ӵ��� ��Ʈ�� �ϱ� ���� imgui
	if(ImGui::Begin(WideToMultiU8(L"���α׷� ����").c_str()))  // Begin
	{
		ImGui::SliderFloat("Speed Factor", &speed_factor, 0.0f, 4.0f);
		ImGui::Text(WideToMultiU8(L"���α׷� ��� %.3f ms/frame (%.1f FPS)").c_str(), 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::InputText(WideToMultiU8(L"�ؽ�Ʈ �Է�").c_str(), buffer, sizeof(buffer));
	}
	ImGui::End();                                              // End

	wnd.Gfx().EndFrame();
}

App::~App()
{}

// ���ø����̼� ���� �Լ�.
int App::Go()
{
	while (true)
	{
		if (const auto ecode = Window::ProcessMessages())
		{
			// ProcessMessages() �Լ��� ���� ���� �� �ִ� optional ��ü�� �޴� ���,
			// ������ ������ �ǹ��ϹǷ� exit code�� ���� ����.
			return *ecode; // optional ��ü�� ��� �ִ� ���� ������ �� *�����ڸ� ���.
		}
		DoFrame();
	}
}

