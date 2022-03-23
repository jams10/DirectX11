#include "App.h"
#include <sstream>
#include <iomanip>

App::App()
	:
	wnd(800, 600, L"윈도우!")
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

// 한 프레임에 대한 작업들을 처리하는 함수.
void App::DoFrame()
{
	gt.Tick();
	const float c = sin(gt.GetTotalTime() / 2.0f + 0.5f);
	wnd.Gfx().ClearBuffer(c, c, 1.0f);
	wnd.Gfx().DrawTestTriangle(
		-gt.GetTotalTime(),
		0.0f,
		0.0f
	);
	wnd.Gfx().DrawTestTriangle(
		gt.GetTotalTime(),
		wnd.mouse.GetPosX() / 400.f - 1.0f,
		-wnd.mouse.GetPosY() / 300.f + 1.0f
	);
	wnd.Gfx().EndFrame();
}