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
	const float t = gt.GetTotalTime();
	std::wostringstream oss;
	oss << L"Time Elapsed : " << std::setprecision(1) << std::fixed << t << L" s";
	wnd.SetTitle(oss.str());
}