#include "App.h"
#include <memory>
#include "CustomMath.h"
#include "GDIPlusManager.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "StringEncoding.h"
#include "VertexBuffer.h"

GDIPlusManager gdipm; // GDI+ ���̺귯���� ����ϱ� ���� �ռ� �ʱ�ȭ ���־�� ��. ������ ȣ���� ���� �ʱ�ȭ�� ����.

App::App()
	:
	wnd(800, 600, L"������!"),
	aspectRatio(800.f / 600.f),
	nearZ(0.5f),
	farZ(40.f),
	light(wnd.Gfx())
{
	//wall.SetRootTransform(DirectX::XMMatrixTranslation(-1.5f, 0.0f, 0.0f));
	//tp.SetPos({ 1.5f,0.0f,0.0f });
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
	//wall.Draw(wnd.Gfx());
	//tp.Draw(wnd.Gfx());
	//nano.Draw( wnd.Gfx() );
	gobber.Draw(wnd.Gfx());
	light.Draw(wnd.Gfx());

	while (const auto e = wnd.kbd.ReadKey())
	{
		if (!e->IsPress())
		{
			continue;
		}

		switch (e->GetCode())
		{
		case VK_ESCAPE:
			if (wnd.CursorEnabled())
			{
				wnd.DisableCursor();
				wnd.mouse.EnableRaw();
			}
			else
			{
				wnd.EnableCursor();
				wnd.mouse.DisableRaw();
			}
			break;
		case VK_F1:
			show_demo_window = true;
			break;
		}
	}
#pragma region CameraMove
	if (!wnd.CursorEnabled())
	{
		if (wnd.kbd.KeyIsPressed('W'))
		{
			cam.Translate({ 0.0f,0.0f,dt });
		}
		if (wnd.kbd.KeyIsPressed('A'))
		{
			cam.Translate({ -dt,0.0f,0.0f });
		}
		if (wnd.kbd.KeyIsPressed('S'))
		{
			cam.Translate({ 0.0f,0.0f,-dt });
		}
		if (wnd.kbd.KeyIsPressed('D'))
		{
			cam.Translate({ dt,0.0f,0.0f });
		}
		if (wnd.kbd.KeyIsPressed('R'))
		{
			cam.Translate({ 0.0f,dt,0.0f });
		}
		if (wnd.kbd.KeyIsPressed('F'))
		{
			cam.Translate({ 0.0f,-dt,0.0f });
		}
	}
#pragma endregion

	while (const auto delta = wnd.mouse.ReadRawDelta())
	{
		if (!wnd.CursorEnabled())
		{
			cam.Rotate((float)delta->x, (float)delta->y);
		}
	}

	// imgui windows
	cam.SpawnControlWindow();
	light.SpawnControlWindow();
	ShowImguiDemoWindow();
	gobber.ShowWindow(wnd.Gfx(), "gobber");
	//wall.ShowWindow("Wall");
	//tp.SpawnControlWindow(wnd.Gfx());
	//nano.ShowWindow( "Model 1" );

	wnd.Gfx().EndFrame();
}

void App::ShowImguiDemoWindow()
{
	if (show_demo_window)
	{
		ImGui::ShowDemoWindow(&show_demo_window);
	}
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

