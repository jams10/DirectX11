#include "Window.h"
#include <sstream>
#include "../resource.h" // �������� ��� ���� �ڿ����� ������ ����.
#include "WindowsThrowMacros.h"
#include "imgui_impl_win32.h"

#pragma region WindowClass
// ������ Ŭ����
Window::WindowClass Window::WindowClass::wndClass; // static ��� ���� ����
// WindowClass ������. ������ Ŭ���� ������ ���.
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
	wc.hIcon = static_cast<HICON>(LoadImage(
		GetInstance(), MAKEINTRESOURCE(IDI_ICON1),
		IMAGE_ICON, 32, 32, 0));
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm = static_cast<HICON>(LoadImage(
		GetInstance(), MAKEINTRESOURCE(IDI_ICON1),
		IMAGE_ICON, 16, 16, 0));
	RegisterClassEx(&wc);
}
// WindowClass �Ҹ���. ����� ������ ���� ���.
Window::WindowClass::~WindowClass()
{
	UnregisterClass(wndClassName, GetInstance()); // ����� �����츦 ���� ����.
}
// ������ Ŭ������ �̸��� �������ִ� �Լ�.
const wchar_t* Window::WindowClass::GetName() noexcept
{
	return wndClassName;
}
// ���α׷� �ν��Ͻ��� ���� �ڵ��� �������ִ� �Լ�.
HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInst;
}
#pragma endregion

#pragma region Window
// Window Ŭ���� ������. ������ ���� �� ����
Window::Window(int width, int height, const wchar_t* name)
	:
	width(width),
	height(height)
{
	// ���ϴ� client ���� ũ�⿡ ���缭 winodw�� ũ�⸦ �������.
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;

	// ������ ������ ����.
	if (!AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE))
	{
		throw WND_LAST_EXCEPT();
	}
	
	// �����츦 �����ϰ� �����쿡 ���� �ڵ��� ����.
	hWnd = CreateWindow(
		WindowClass::GetName(), name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top,
		nullptr, nullptr, WindowClass::GetInstance(), this
	);

	// ������ ������ ������ ���
	if (hWnd == nullptr)
	{
		throw WND_LAST_EXCEPT();
	}
	
	// ������ ȭ�鿡 ����.
	ShowWindow(hWnd, SW_SHOWDEFAULT);

	// Imgui Win32 ������ �ʱ�ȭ.
	ImGui_ImplWin32_Init(hWnd);

	// Graphics ��ü ����.
	pGfx = std::make_unique<Graphics>(hWnd, width, height);
}

// Window Ŭ���� �Ҹ���. ������ ������ �ı�.
Window::~Window()
{
	ImGui_ImplWin32_Shutdown();
	DestroyWindow(hWnd);
}

// ������ Ÿ��Ʋ �̸� �������ִ� �Լ�.
void Window::SetTitle(const std::wstring& title)
{
	if (SetWindowText(hWnd, title.c_str()) == 0)
	{
		throw WND_LAST_EXCEPT();
	}
}

// ������ �޽��� ����. ������ �޽����� ���ν����� �����ִ� �Լ�.
std::optional<int> Window::ProcessMessages() noexcept
{
	MSG msg;
	// �޽��� ť�� �޽����� ������, �ش� �޽����� �����ϰ� ���ν����� ������.(ť�� ����־ ��� ���¿� ���� ����.)
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		// GetMessage�� �ٸ��� PeekMessage�� ���� WM_QUIT �޽����� ���� üũ�� �־�� ��.
		if (msg.message == WM_QUIT)
		{
			// PostQuitMessage �Լ��� ������ ���� �ڵ尡 wParam�ȿ� �ִµ� �� �ڵ带 ����ִ� optional ��ü�� ������.
			return msg.wParam;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// ���� ������ �ʴ� ��� �� optional�� ������. �� optional�� nullopt ��. if������ üũ�ϸ� if(false)�� ����ϰ� ������.
	return {};
}

// Graphics ��ü�� ���� ������ �������ִ� �Լ�.
Graphics& Window::Gfx()
{
	if (!pGfx)
	{
		throw WND_NOGFX_EXCEPT();
	}
	return *pGfx;
}

// ���� ������� ��� �Լ��� ������ ���ν����� ����ϱ� ���� �⺻ ������ ����ϴ� �Լ�.
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

// ������ �޽����� �츮�� �ۼ��� ��� �Լ��� �����ϱ� ���� �Լ�.
LRESULT CALLBACK Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// window class�� ���� �����͸� ����.
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	
	// window class �ڵ鷯�� �޽����� ������.
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

// ���� ������ �޽����� ó���� �� �Լ�.
LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// ������ �޽����� ������, imgui�� ���� ó���ϵ��� ��. (���� ���� ��.)
	// �׷���, �׷��� �ش� �޽����� imgui�� ���ν����� ó���ϰ� �Ǹ�(true�� ����) �츮�� HandleMsg������ ó���� �� �����Ƿ�, �ٷ� �����ؼ� ���ν��� ��������.
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		return true;
	}

	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);                    	 
		return 0;
	case WM_KILLFOCUS:    // �����쿡 ���� ��Ŀ���� �Ҿ������ Ű �Է� ���¸� ��� �ʱ�ȭ ����.
		kbd.ClearState();
		break;
#pragma region KeyboardMSG
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (!(lParam & 0x40000000) || kbd.AutorepeatIsEnabled())
		{
			kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		kbd.OnKeyReleased(static_cast<unsigned char>(wParam));
		break;
	case WM_CHAR:
		kbd.OnChar(static_cast<unsigned char>(wParam));
		break;
#pragma endregion
#pragma region MouseMSG
	case WM_MOUSEMOVE:
	{
		const POINTS pt = MAKEPOINTS(lParam);

		// ���콺 ��ǥ�� Ŭ���̾�Ʈ ���� �ȿ� �ִ� ���
		if (pt.x >= 0 && pt.x < width && pt.y >= 0 && pt.y < height)
		{
			mouse.OnMouseMove(pt.x, pt.y);    // ���콺 ��ǥ�� ����.
			if (!mouse.IsInWindow())          // ���콺�� ������ �����ȿ� �ִ� ���°� �ƴ� ��쿴�ٸ�,
			{
				SetCapture(hWnd);             // ���콺 ĸ��
				mouse.OnMouseEnter();         // ���콺�� Ŭ���̾�Ʈ ���� �ȿ� �ִ� ���·� �ٲ���.
			}
		}

		// ���콺 ��ǥ�� Ŭ���̾�Ʈ ���� �ۿ� �ִ� ���
		else
		{
			if (wParam & (MK_LBUTTON | MK_RBUTTON)) // ���� ���콺�� ���� ���·� �ٱ��� �����ٸ�, ĸ�ĸ� �����ϰ�, ���콺 ��ǥ ��������.
			{
				mouse.OnMouseMove(pt.x, pt.y);
			}
			else                                    // ���콺�� ������ �ʾҴٸ� ĸ�ĸ� Ǯ���ְ�, ���� ������ ���� ���·� �ٲ���.
			{
				ReleaseCapture();
				mouse.OnMouseLeave();
			}
		}
		break;
	}
	case WM_LBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftPressed(pt.x, pt.y);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightPressed(pt.x, pt.y);
		break;
	}
	case WM_LBUTTONUP:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftReleased(pt.x, pt.y);
		// ���� ������ �����ٸ� ���콺 ĸ�ĸ� Ǯ����.
		if (pt.x < 0 || pt.x >= width || pt.y < 0 || pt.y >= height)
		{
			ReleaseCapture();
			mouse.OnMouseLeave();
		}
		break;
	}
	case WM_RBUTTONUP:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightReleased(pt.x, pt.y);
		// ���� ������ �����ٸ� ���콺 ĸ�ĸ� Ǯ����.
		if (pt.x < 0 || pt.x >= width || pt.y < 0 || pt.y >= height)
		{
			ReleaseCapture();
			mouse.OnMouseLeave();
		}
		break;
	}
	case WM_MOUSEWHEEL:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		mouse.OnWheelDelta(pt.x, pt.y, delta);
		break;
	}
#pragma endregion
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
#pragma endregion

#pragma region Exception
// ���� �ڵ带 ���ڿ��� ��ȯ���ִ� �Լ�.
std::string Window::Exception::TranslateErrorCode(HRESULT hr) noexcept
{
	char* pMsgBuf = nullptr;
   /*
		<FormatMessage �Լ�>
		HRESULT ���� �޾Ƽ� ���� �ڵ带 ���� �޽����� ��ȯ����.
		�����찡 ���� ���ڿ��� ���� �޸𸮸� �Ҵ��ϰ�, ���ڷ� �ѱ� �����Ͱ� �̸� ����Ű���� ����.
	    FORMAT_MESSAGE_ALLOCATE_BUFFER : os�� �˾Ƽ� �޽��� ���� ������ �Ҵ��ϵ��� ��.
		FORMAT_MESSAGE_FROM_SYSTEM     : os�� ���� ���� �޽��� ������.
		FORMAT_MESSAGE_IGNORE_INSERTS  : �޽��� ���� ���� ������ ����.
	*/
	const DWORD nMsgLen = FormatMessageA( // WinMain���� MessageBoxA�� ��Ƽ����Ʈ ������ ����ϰ� �־� FormatMessage�� ��Ƽ����Ʈ ������ �����.
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr
	);
	// ���ڿ� ���̰� 0�̸� ���и� ��Ÿ��.
	if (nMsgLen == 0)
	{
		return "Unidentified error code";
	}
	// ���� ���ڿ��� �����찡 �Ҵ����� ���ۿ��� std::string���� ��������.
	std::string errorString = pMsgBuf;
	// free windows buffer
	/*
		<LocalFree �Լ�>
		Ư�� ���� �޸� ������Ʈ�� �����ϰ�, �� �ڵ��� ��ȿ���� �ʵ��� �ٲ�.
		���� �޽����� ���� �Ҵ��� ������ ����.
	*/
	LocalFree(pMsgBuf);
	return errorString;
}

Window::HrException::HrException(int line, const char* file, HRESULT hr) noexcept
	:
	Exception(line, file),
	hr(hr)
{}

// what() : ����� ���� ���ڿ��� ������ �����ϴ� �Լ�. std::exception���� ���� �������̵�.
const char* Window::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

// ���� Ÿ�� ���ڿ��� �������ִ� �Լ�.
const char* Window::HrException::GetType() const noexcept
{
	return "Custom Window Exception";
}

// ���� �ڵ�(HRESULT)�� �������ִ� �Լ�.
HRESULT Window::HrException::GetErrorCode() const noexcept
{
	return hr;
}

// dxerr ���̺귯���� ���� ������ ���� ���� ���ڿ��� �������ִ� �Լ�.
std::string Window::HrException::GetErrorDescription() const noexcept
{
	return Exception::TranslateErrorCode(hr);
}

// ���� Ÿ�� ���ڿ��� �������ִ� �Լ�.
const char* Window::NoGfxException::GetType() const noexcept
{
	return "Custom Window Exception [No Graphics]";
}
#pragma endregion