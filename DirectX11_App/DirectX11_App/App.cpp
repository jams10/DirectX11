#include "App.h"
#include <memory>
#include "CustomMath.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "StringEncoding.h"
#include "VertexBuffer.h"
#include "CustomUtil.h"
#include "DynamicConstant.h"

App::App(const std::string& commandLine)
	:
	commandLine(commandLine),
	wnd(1280, 720, L"윈도우!"),
	scriptCommander(TokenizeQuoted(commandLine)),
	aspectRatio(720.f / 1280.f),
	nearZ(0.5f),
	farZ(400.f),
	light(wnd.Gfx())
{

	//wall.SetRootTransform(DirectX::XMMatrixTranslation(-12.0f, 0.0f, 0.0f));
	//tp.SetPos({ 12.0f,0.0f,0.0f });
	//gobber.SetRootTransform(DirectX::XMMatrixTranslation(0.0f, 0.0f, -4.0f));
	//nano.SetRootTransform(DirectX::XMMatrixTranslation(0.0f, -7.0f, 6.0f));
	//bluePlane.SetPos( cam.GetPos() );
	//redPlane.SetPos(cam.GetPos());

	// 투영 행렬
	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, aspectRatio, nearZ, farZ));
};

void App::DoFrame()
{
	gt.Tick();
	const auto dt = gt.GetDeltaTime() * speed_factor;

	// 윈도우 크기가 변하면 Projection 행렬 값도 변해야 함.
	std::pair<UINT, UINT> windowSize = wnd.GetWindowSize();
	aspectRatio = static_cast<float>(windowSize.first) / windowSize.second;
	// FOVLH가 함수가 아님. 첫번째 매개변수가 view width, 두번째 매개변수가 view height
	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 1.f / aspectRatio, nearZ, farZ));

	wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
	wnd.Gfx().SetCamera(cam.GetMatrix());   // 카메라 행렬을 얻어옴. 카메라를 이동하면 뷰 변환 행렬도 달라지기 때문에 매 프레임 마다 얻어옴.
	light.Bind(wnd.Gfx(), cam.GetMatrix()); // 라이트 업데이트.
	
	// 3D 모델 그리기
	//wall.Draw(wnd.Gfx());
	//tp.Draw(wnd.Gfx());
	//nano.Draw( wnd.Gfx() );
	//gobber.Draw(wnd.Gfx());
	light.Draw(wnd.Gfx());
	sponza.Draw(wnd.Gfx());
	cube.Draw(wnd.Gfx());
	cube2.Draw(wnd.Gfx());
	cube.DrawOutline(wnd.Gfx());
	cube2.DrawOutline(wnd.Gfx());
	//bluePlane.Draw(wnd.Gfx());
	//redPlane.Draw(wnd.Gfx());

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
	cube.SpawnControlWindow(wnd.Gfx(), "Cube 1");
	cube2.SpawnControlWindow(wnd.Gfx(), "Cube 2");
	//gobber.ShowWindow(wnd.Gfx(), "gobber");
	//wall.ShowWindow(wnd.Gfx(), "Wall");
	//tp.SpawnControlWindow(wnd.Gfx());
	//nano.ShowWindow(wnd.Gfx(), "Model 1");
	sponza.ShowWindow(wnd.Gfx(), "Sponza");
	//bluePlane.SpawnControlWindow(wnd.Gfx(), "Blue Plane");
	//redPlane.SpawnControlWindow(wnd.Gfx(), "Red Plane");


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

// 애플리케이션 루프 함수.
int App::Go()
{
	while (true)
	{
		if (const auto ecode = Window::ProcessMessages())
		{
			// ProcessMessages() 함수를 통해 값이 들어가 있는 optional 객체를 받는 경우,
			// 종료할 것임을 의미하므로 exit code를 리턴 해줌.
			return *ecode; // optional 객체가 들고 있는 값에 접근할 때 *연산자를 사용.
		}
		DoFrame();
	}
}

