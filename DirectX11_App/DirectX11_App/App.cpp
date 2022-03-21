#include "App.h"
#include <sstream>
#include <iomanip>

App::App()
	:
	wnd(800, 600, L"The Donkey Fart Box")
{}

// 애플리케이션 루프 함수.
int App::Go()
{
	MSG msg;
	BOOL gResult;
	while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		DoFrame();
	}

	// wParam here is the value passed to PostQuitMessage
	return msg.wParam;
}

// 한 프레임에 대한 작업들을 처리하는 함수.
void App::DoFrame()
{
	gt.Tick();
	const float t = gt.GetTotalTime();
	std::wostringstream oss;
	oss << L"Time Elapsed : " << std::setprecision(1) << std::fixed << t << L" s";
	wnd.SetTitle(oss.str());
}