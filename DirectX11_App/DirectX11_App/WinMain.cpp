#include <Windows.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
			PostQuitMessage(37);
			break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int CALLBACK WinMain(
	HINSTANCE hInstance,     // 우리가 작성한 프로그램 인스턴스에 대한 핸들.
	HINSTANCE hPrevInstance, // 현재는 사용하지 않음. 무조건 NULL.
	LPSTR	  lpCmdLine,     // 명령줄. C프로그램 에서의 char** argv의 윈도우 애플리케이션 버전.
	int       nCmdShow)      // 윈도우를 시작할 때의 형태. 최대/최소화 여부 등을 설정.
{
	const wchar_t* pClassName = L"WindowClass";

	// 윈도우 클래스 등록
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = pClassName;
	wc.hIconSm = nullptr;
	RegisterClassEx(&wc);

	// 윈도우 생성
	HWND hWnd = CreateWindowEx(
		0,
		pClassName,
		L"윈도우!",
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		200, 200,
		640, 480,
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);

	// 윈도우 보이기
	ShowWindow(hWnd, SW_SHOW);

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
	else
	{
		return msg.wParam;
	}
	
return 0;
}