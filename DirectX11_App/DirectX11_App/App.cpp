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
	wnd(1080, 720, L"������!"),
	viewHeight(720.f / 1080.f),
	nearZ(0.5f),
	farZ(40.f),
	light(wnd.Gfx())
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
			// material ���� ���� �������� ����.
			const DirectX::XMFLOAT3 mat = { cdist(rng),cdist(rng),cdist(rng) };
			return std::make_unique<Box>(
				gfx, rng, adist, ddist,
				odist, rdist, bdist, mat
				);
		}
	private:
		Graphics& gfx;
		std::mt19937 rng{ std::random_device{}() };
		std::uniform_real_distribution<float> adist{ 0.0f,PI * 2.0f };
		std::uniform_real_distribution<float> ddist{ 0.0f,PI * 0.5f };
		std::uniform_real_distribution<float> odist{ 0.0f,PI * 0.08f };
		std::uniform_real_distribution<float> rdist{ 6.0f,20.0f };
		std::uniform_real_distribution<float> bdist{ 0.4f,3.0f };
		std::uniform_real_distribution<float> cdist{ 0.0f,1.0f };
	};

	drawables.reserve(nDrawables);
	std::generate_n(std::back_inserter(drawables), nDrawables, Factory{ wnd.Gfx() });

	// ���� ���
	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, viewHeight, nearZ, farZ));
};

void App::DoFrame()
{
	gt.Tick();
	const auto dt = gt.GetDeltaTime() * speed_factor;

	// ������ ũ�Ⱑ ���ϸ� Projection ��� ���� ���ؾ� ��.
	std::pair<UINT, UINT> windowSize = wnd.GetWindowSize();
	viewHeight = static_cast<float>(windowSize.second) / windowSize.first;
	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, viewHeight, nearZ, farZ));

	wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
	wnd.Gfx().SetCamera(cam.GetMatrix()); // ī�޶� ����� ����. ī�޶� �̵��ϸ� �� ��ȯ ��ĵ� �޶����� ������ �� ������ ���� ����.
	light.Bind(wnd.Gfx());                // ����Ʈ ������Ʈ.
	
	// ������ ������
	for (auto& d : drawables)
	{
		d->Update(wnd.kbd.KeyIsPressed(VK_SPACE) ? 0.0f : dt);
		d->Draw(wnd.Gfx());
	}
	light.Draw(wnd.Gfx());

	static char buffer[1024];

	// �ùķ��̼� �ӵ��� ��Ʈ�� �ϱ� ���� ui ����.
	if(ImGui::Begin(WideToMultiU8(L"���α׷� ����").c_str()))  // Begin
	{
		ImGui::SliderFloat("Speed Factor", &speed_factor, 0.0f, 4.0f);
		ImGui::Text(WideToMultiU8(L"���α׷� ��� %.3f ms/frame (%.1f FPS)").c_str(), 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Status: %s", wnd.kbd.KeyIsPressed(VK_SPACE) ? "PAUSED" : "RUNNING (hold spacebar to pause)");
	}
	ImGui::End();                                              // End

	// ī�޶�� ����Ʈ�� ������ ui ����.
	cam.SpawnControlWindow();
	light.SpawnControlWindow();

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

