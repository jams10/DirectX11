#include "App.h"
#include <memory>
#include "CustomMath.h"
#include "GDIPlusManager.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "StringEncoding.h"
#include "VertexBuffer.h"
#include "TexturePreprocessor.h"
#include <shellapi.h>

GDIPlusManager gdipm; // GDI+ ���̺귯���� ����ϱ� ���� �ռ� �ʱ�ȭ ���־�� ��. ������ ȣ���� ���� �ʱ�ȭ�� ����.

App::App(const std::string& commandLine)
	:
	commandLine(commandLine),
	wnd(1280, 720, L"������!"),
	aspectRatio(720.f / 1280.f),
	nearZ(0.5f),
	farZ(400.f),
	light(wnd.Gfx())
{
	// makeshift cli for doing some preprocessing bullshit (so many hacks here)
	if (this->commandLine != "")
	{
		int nArgs;
		const auto pLineW = GetCommandLineW();
		const auto pArgs = CommandLineToArgvW(pLineW, &nArgs);
		if (nArgs >= 3 && std::wstring(pArgs[1]) == L"--twerk-objnorm")
		{
			const std::wstring pathInWide = pArgs[2];
			TexturePreprocessor::FlipYAllNormalMapsInObj(
				std::string(pathInWide.begin(), pathInWide.end())
			);
			throw std::runtime_error("Normal maps all processed successfully. Just kidding about that whole runtime error thing.");
		}
		else if (nArgs >= 3 && std::wstring(pArgs[1]) == L"--twerk-flipy")
		{
			const std::wstring pathInWide = pArgs[2];
			const std::wstring pathOutWide = pArgs[3];
			TexturePreprocessor::FlipYNormalMap(
				std::string(pathInWide.begin(), pathInWide.end()),
				std::string(pathOutWide.begin(), pathOutWide.end())
			);
			throw std::runtime_error("Normal map processed successfully. Just kidding about that whole runtime error thing.");
		}
		else if (nArgs >= 4 && std::wstring(pArgs[1]) == L"--twerk-validate")
		{
			const std::wstring minWide = pArgs[2];
			const std::wstring maxWide = pArgs[3];
			const std::wstring pathWide = pArgs[4];
			TexturePreprocessor::ValidateNormalMap(
				std::string(pathWide.begin(), pathWide.end()), std::stof(minWide), std::stof(maxWide)
			);
			throw std::runtime_error("Normal map validated successfully. Just kidding about that whole runtime error thing.");
		}
	}

	//wall.SetRootTransform(DirectX::XMMatrixTranslation(-12.0f, 0.0f, 0.0f));
	//tp.SetPos({ 12.0f,0.0f,0.0f });
	//gobber.SetRootTransform(DirectX::XMMatrixTranslation(0.0f, 0.0f, -4.0f));
	//nano.SetRootTransform(DirectX::XMMatrixTranslation(0.0f, -7.0f, 6.0f));
	bluePlane.SetPos( cam.GetPos() );
	redPlane.SetPos(cam.GetPos());

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
	//gobber.Draw(wnd.Gfx());
	light.Draw(wnd.Gfx());
	sponza.Draw(wnd.Gfx());
	bluePlane.Draw(wnd.Gfx());
	redPlane.Draw(wnd.Gfx());

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
	//gobber.ShowWindow(wnd.Gfx(), "gobber");
	//wall.ShowWindow(wnd.Gfx(), "Wall");
	//tp.SpawnControlWindow(wnd.Gfx());
	//nano.ShowWindow(wnd.Gfx(), "Model 1");
	sponza.ShowWindow(wnd.Gfx(), "Sponza");
	bluePlane.SpawnControlWindow(wnd.Gfx(), "Blue Plane");
	redPlane.SpawnControlWindow(wnd.Gfx(), "Red Plane");

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

