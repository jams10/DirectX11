#include "App.h"
#include <memory>
#include "CustomMath.h"
#include "GDIPlusManager.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "StringEncoding.h"

GDIPlusManager gdipm; // GDI+ ���̺귯���� ����ϱ� ���� �ռ� �ʱ�ȭ ���־�� ��. ������ ȣ���� ���� �ʱ�ȭ�� ����.

App::App()
	:
	wnd(800, 600, L"������!"),
	aspectRatio(800.f / 600.f),
	nearZ(0.5f),
	farZ(40.f),
	light(wnd.Gfx())
{
	// ���� ���
	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, aspectRatio, nearZ, farZ));
};

void App::DoFrame()
{
	gt.Tick();
	const auto dt = gt.GetDeltaTime() * speed_factor;

	// ������ ũ�Ⱑ ���ϸ� Projection ��� ���� ���ؾ� ��.
	std::pair<UINT, UINT> windowSize = wnd.GetWindowSize();
	aspectRatio = static_cast<float>(windowSize.first) / windowSize.second;
	// FOVLH�� �Լ��� �ƴ�. ù��° �Ű������� view width, �ι�° �Ű������� view height
	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 1.f / aspectRatio, nearZ, farZ));

	wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
	wnd.Gfx().SetCamera(cam.GetMatrix());   // ī�޶� ����� ����. ī�޶� �̵��ϸ� �� ��ȯ ��ĵ� �޶����� ������ �� ������ ���� ����.
	light.Bind(wnd.Gfx(), cam.GetMatrix()); // ����Ʈ ������Ʈ.
	
	// 3D �� �׸���
	nano.Draw(wnd.Gfx());
	
	light.Draw(wnd.Gfx());

	// imgui windows
	cam.SpawnControlWindow();
	light.SpawnControlWindow();
	ShowImguiDemoWindow();
	nano.ShowWindow();
	ShowRawInputWindow();

	wnd.Gfx().EndFrame();
}

void App::ShowImguiDemoWindow()
{
	static bool show_demo_window = true;
	if (show_demo_window)
	{
		ImGui::ShowDemoWindow(&show_demo_window);
	}
}

void App::ShowRawInputWindow()
{
	while (const auto d = wnd.mouse.ReadRawDelta())
	{
		x += d->x;
		y += d->y;
	}
	if (ImGui::Begin("Raw Input"))
	{
		ImGui::Text("Tally: (%d,%d)", x, y);
	}
	ImGui::End();
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

