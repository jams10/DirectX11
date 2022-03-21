#include "Window.h"
#include <sstream>

// WinMain : 윈도우 응용 프로그램의 진입점.
int CALLBACK WinMain(
	HINSTANCE hInstance,     // 우리가 작성한 프로그램 인스턴스에 대한 핸들.
	HINSTANCE hPrevInstance, // 현재는 사용하지 않음. 무조건 NULL.
	LPSTR	  lpCmdLine,     // 명령줄. C프로그램 에서의 char** argv의 윈도우 애플리케이션 버전.
	int       nCmdShow)      // 윈도우를 시작할 때의 형태. 최대/최소화 여부 등을 설정.
{
	try
	{
		Window wnd(800, 600, L"윈도우!");

		// 메시지 받기
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