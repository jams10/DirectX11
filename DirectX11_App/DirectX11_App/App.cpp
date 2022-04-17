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

GDIPlusManager gdipm; // GDI+ 라이브러리를 사용하기 위해 앞서 초기화 해주어야 함. 생성자 호출을 통해 초기화를 진행.

App::App()
	:
	wnd(1080, 720, L"윈도우!"),
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
			return std::make_unique<Box>(
				gfx, rng, adist, ddist,
				odist, rdist, bdist
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
	};

	drawables.reserve(nDrawables);
	std::generate_n(std::back_inserter(drawables), nDrawables, Factory{ wnd.Gfx() });

	// 투영 행렬
	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, viewHeight, nearZ, farZ));
};

void App::DoFrame()
{
	gt.Tick();
	const auto dt = gt.GetDeltaTime() * speed_factor;

	wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
	wnd.Gfx().SetCamera(cam.GetMatrix()); // 카메라 행렬을 얻어옴. 카메라를 이동하면 뷰 변환 행렬도 달라지기 때문에 매 프레임 마다 얻어옴.
	light.Bind(wnd.Gfx());                // 라이트 업데이트.
	
	// 윈도우 크기가 변하면 Projection 행렬 값도 변해야 함.
	std::pair<UINT, UINT> windowSize = wnd.GetWindowSize();
	viewHeight = static_cast<float>(windowSize.second) / windowSize.first;
	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, viewHeight, nearZ, farZ));

	// 도형들 렌더링
	for (auto& d : drawables)
	{
		d->Update(wnd.kbd.KeyIsPressed(VK_SPACE) ? 0.0f : dt);
		d->Draw(wnd.Gfx());
	}
	light.Draw(wnd.Gfx());

	static char buffer[1024];

	// 시뮬레이션 속도를 컨트롤 하기 위한 ui 생성.
	if(ImGui::Begin(WideToMultiU8(L"프로그램 상태").c_str()))  // Begin
	{
		ImGui::SliderFloat("Speed Factor", &speed_factor, 0.0f, 4.0f);
		ImGui::Text(WideToMultiU8(L"프로그램 평균 %.3f ms/frame (%.1f FPS)").c_str(), 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Status: %s", wnd.kbd.KeyIsPressed(VK_SPACE) ? "PAUSED" : "RUNNING (hold spacebar to pause)");
	}
	ImGui::End();                                              // End

	// 카메라와 라이트를 조작할 ui 생성.
	cam.SpawnControlWindow();
	light.SpawnControlWindow();

	wnd.Gfx().EndFrame();
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

