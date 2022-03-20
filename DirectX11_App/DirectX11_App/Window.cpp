#include "Window.h"


#pragma region WindowClass
// ������ Ŭ����
Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept
	:
	hInst(GetModuleHandle(nullptr)) // exe ���Ͽ����� �� �Լ��� ������ �ν��Ͻ� �ڵ��̳� WinMain()�� �Ű������� hInstance�� ���� �ٸ��� �ʴ�. 
	                                // �׷���, DLL �ȿ� ��������� ������ ��� WinMain�� hInstance�� DLL�� hInstance��, GetModuleHandle�� ������ DLL�� �ε��� exe ������ HINSTANCE�� ��ȯ �Ѵ�.
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm = nullptr;
	RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass()
{
	UnregisterClass(wndClassName, GetInstance()); // ����� �����츦 ���� ����.
}

const wchar_t* Window::WindowClass::GetName() noexcept
{
	return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInst;
}
#pragma endregion

#pragma region Window
// ������ ���� �� ����
Window::Window(int width, int height, const wchar_t* name) noexcept
{
	// ���ϴ� client ���� ũ�⿡ ���缭 winodw�� ũ�⸦ �������.
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;

	// ������ ������ ����.
	AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);
	
	// �����츦 �����ϰ� �����쿡 ���� �ڵ��� ����.
	hWnd = CreateWindow(
		WindowClass::GetName(), name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top,
		nullptr, nullptr, WindowClass::GetInstance(), this
	);
	
	// ������ ȭ�鿡 ����.
	ShowWindow(hWnd, SW_SHOWDEFAULT);
}

Window::~Window()
{
	DestroyWindow(hWnd);
}

LRESULT CALLBACK Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// CreateWindow()�� ������ ���ڷ� �Ѱ��� this �����͸� �̿��� ������ API �ʿ��ִ� ������ Ŭ���� �����͸� ������.
	if (msg == WM_NCCREATE)
	{
		// ���� �����ͷ� ���� ������ Ŭ������ ���� �����͸� ����.
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);

		// WinAPI�� ���� �����Ǵ� ����� �����͸� ������ Ŭ������ ���� �����͸� �����ϵ��� ������.
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		
		// �޽��� ���ν����� �Ϲ� �ڵ鷯�� �����ϰ� �޽����� �޾Ƶ��� �غ� ��.
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
		
		// window class �ڵ鷯�� �޽����� ������.
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}

	// WN_NCCREATE �޽��� ���� �޽����� �޴´ٸ�, �Ϲ� �ڵ鷯�� �޽����� ó��.
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// window class�� ���� �����͸� ����.
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	
	// window class �ڵ鷯�� �޽����� ������.
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);  // 0�� ������ while ���� ����������, Window Ŭ������ �Ҹ��ڸ� ȣ�� �Ͽ� DestroyWindow �Լ��� �� ���� �����츦 ������ ��.
                             // PostQuitMessage �Լ��� �������� �޽��� ť�� WM_QUIT �޽����� �����ϰ� ��� ������.
		                     // QUIT �޽����� �����ϰ� DefWindowProc�� DestroyWindow�� ȣ���ϴ°� �ƴ϶�, 
							 // WindowManager ��ü�� �Ҹ� �� �� DestroyWindow�� ȣ���ϵ��� �� �ߺ����� DestroyWindow�� ȣ������ �ʵ��� ��.
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
#pragma endregion