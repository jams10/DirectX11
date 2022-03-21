#include "App.h"
#include <sstream>
#include <iomanip>

App::App()
	:
	wnd(800, 600, L"������!")
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

// �� �����ӿ� ���� �۾����� ó���ϴ� �Լ�.
void App::DoFrame()
{
	gt.Tick();
	const float c = sin(gt.GetTotalTime() / 2.0f + 0.5f);
	wnd.Gfx().ClearBuffer(c, c, 1.0f);
	wnd.Gfx().EndFrame();
}