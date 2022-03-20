#include "Window.h"

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