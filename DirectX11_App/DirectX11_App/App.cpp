#include "App.h"
#include <memory>
#include "CustomMath.h"
#include "GDIPlusManager.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "StringEncoding.h"

GDIPlusManager gdipm; // GDI+ 라이브러리를 사용하기 위해 앞서 초기화 해주어야 함. 생성자 호출을 통해 초기화를 진행.

App::App()
	:
	wnd(800, 600, L"윈도우!"),
	aspectRatio(800.f / 600.f),
	nearZ(0.5f),
	farZ(40.f),
	light(wnd.Gfx())
{
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

