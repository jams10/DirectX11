#include "App.h"
#include <sstream>
#include <iomanip>

App::App()
	:
	wnd(800, 600, L"The Donkey Fart Box")
{}

// ���ø����̼� ���� �Լ�.
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

// �� �����ӿ� ���� �۾����� ó���ϴ� �Լ�.
void App::DoFrame()
{
	gt.Tick();
	const float t = gt.GetTotalTime();
	std::wostringstream oss;
	oss << L"Time Elapsed : " << std::setprecision(1) << std::fixed << t << L" s";
	wnd.SetTitle(oss.str());
}