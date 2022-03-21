#include "Window.h"
#include <sstream>

// WinMain : ������ ���� ���α׷��� ������.
int CALLBACK WinMain(
	HINSTANCE hInstance,     // �츮�� �ۼ��� ���α׷� �ν��Ͻ��� ���� �ڵ�.
	HINSTANCE hPrevInstance, // ����� ������� ����. ������ NULL.
	LPSTR	  lpCmdLine,     // �����. C���α׷� ������ char** argv�� ������ ���ø����̼� ����.
	int       nCmdShow)      // �����츦 ������ ���� ����. �ִ�/�ּ�ȭ ���� ���� ����.
{
	try
	{
		Window wnd(800, 600, L"������!");

		// �޽��� �ޱ�
		MSG msg;
		BOOL gResult;
		while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			while (!wnd.mouse.IsEmpty())
			{
				const auto e = wnd.mouse.Read();
				switch(e.GetType())
				{
				case Mouse::Event::Type::Leave:
					wnd.SetTitle(L"Gone!");
					break;
				case Mouse::Event::Type::Move:
				{
					std::wostringstream oss;
					oss << L"Mouse moved to (" << e.GetPosX() << L"," << e.GetPosY() << L")";
					wnd.SetTitle(oss.str());
				}
				break;
				}
			}
		}

		if (gResult == -1)
		{
			return -1;
		}

		return msg.wParam;
	}
	catch (const CustomException& e)
	{
		MessageBoxA(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		MessageBoxA(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBoxA(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;
}