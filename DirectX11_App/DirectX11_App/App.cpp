#include "App.h"

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

}