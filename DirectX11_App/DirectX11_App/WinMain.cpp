#include <Windows.h>
#include <sstream>
#include "WindowsMessageMap.h"

// WinMain : ������ ���� ���α׷��� ������.
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static WindowsMessageMap mm;
	OutputDebugStringA(mm(msg, wParam, lParam).c_str());

	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(37);
		break;
	case WM_KEYDOWN:
		if (wParam == 'F')
		{
			SetWindowText(hWnd, L"Respects");
		}
		break;
	case WM_KEYUP:
		if (wParam == 'F')
		{
			SetWindowText(hWnd, L"Back to origin");
		}
		break;
	case WM_CHAR:
		{
			static std::wstring title;
			title.push_back(static_cast<char>(wParam));
			SetWindowText(hWnd, title.c_str());
		}
		break;
	case WM_LBUTTONDOWN:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			std::wostringstream oss;
			oss << L"(" << pt.x << L"," << pt.y << L")";
			SetWindowText(hWnd, oss.str().c_str());
		}
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int CALLBACK WinMain(
	HINSTANCE hInstance,     // �츮�� �ۼ��� ���α׷� �ν��Ͻ��� ���� �ڵ�.
	HINSTANCE hPrevInstance, // ����� ������� ����. ������ NULL.
	LPSTR	  lpCmdLine,     // �����. C���α׷� ������ char** argv�� ������ ���ø����̼� ����.
	int       nCmdShow)      // �����츦 ������ ���� ����. �ִ�/�ּ�ȭ ���� ���� ����.
{
	const wchar_t* pClassName = L"WindowClass";

	// ������ Ŭ���� ���
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

	// ������ ����
	HWND hWnd = CreateWindowEx(
		0,
		pClassName,
		L"������!",
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		200, 200,
		640, 480,
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);

	// ������ ���̱�
	ShowWindow(hWnd, SW_SHOW);

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
	else
	{
		return msg.wParam;
	}
	
return 0;
}