#include "Window.h"
#include <sstream>
#include "resource.h" // 아이콘을 비롯 여러 자원들을 얻어오기 위함.

#pragma region WindowClass
// 윈도우 클래스
Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept
	:
	hInst(GetModuleHandle(nullptr)) // exe 파일에서는 이 함수로 얻어오는 인스턴스 핸들이나 WinMain()의 매개변수인 hInstance의 값이 다르지 않다. 
	                                // 그러나, DLL 안에 윈도우들을 생성할 경우 WinMain의 hInstance는 DLL의 hInstance를, GetModuleHandle은 여전히 DLL을 로드한 exe 파일의 HINSTANCE를 반환 한다.
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

Window::WindowClass::~WindowClass()
{
	UnregisterClass(wndClassName, GetInstance()); // 등록한 윈도우를 해제 해줌.
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
// 윈도우 생성 및 설정
Window::Window(int width, int height, const wchar_t* name)
{
	// 원하는 client 영역 크기에 맞춰서 winodw의 크기를 계산해줌.
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;

	// 윈도우 사이즈 조절.
	if (FAILED(AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE)))
	{
		throw WND_LAST_EXCEPT();
	}
	
	// 윈도우를 생성하고 윈도우에 대한 핸들을 얻어옴.
	hWnd = CreateWindow(
		WindowClass::GetName(), name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top,
		nullptr, nullptr, WindowClass::GetInstance(), this
	);

	// 윈도우 생성에 실패한 경우
	if (hWnd == nullptr)
	{
		throw WND_LAST_EXCEPT();
	}
	
	// 윈도우 화면에 띄우기.
	ShowWindow(hWnd, SW_SHOWDEFAULT);
}

Window::~Window()
{
	DestroyWindow(hWnd);
}

LRESULT CALLBACK Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// CreateWindow()의 마지막 인자로 넘겨준 this 포인터를 이용해 윈도우 API 쪽에있는 윈도우 클래스 포인터를 저장함.
	if (msg == WM_NCCREATE)
	{
		// 생성 데이터로 부터 윈도우 클래스에 대한 포인터를 추출.
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);

		// WinAPI에 의해 관리되는 사용자 데이터를 윈도우 클래스에 대한 포인터를 저장하도록 설정함.
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		
		// 메시지 프로시져를 일반 핸들러로 변경하고 메시지를 받아들일 준비를 함.
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
		
		// window class 핸들러에 메시지를 전달함.
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}

	// WN_NCCREATE 메시지 전에 메시지를 받는다면, 일반 핸들러로 메시지를 처리.
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// window class에 대한 포인터를 얻어옴.
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	
	// window class 핸들러에 메시지를 전달함.
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);  // 0을 리턴해 while 문을 빠져나가고, Window 클래스의 소멸자를 호출 하여 DestroyWindow 함수로 한 번만 윈도우를 제거해 줌.
                             // PostQuitMessage 함수는 스레드의 메시지 큐에 WM_QUIT 메시지를 전달하고 즉시 리턴함.
		                     // QUIT 메시지를 전달하고 DefWindowProc가 DestroyWindow를 호출하는게 아니라, 
							 // WindowManager 객체가 소멸 될 때 DestroyWindow를 호출하도록 해 중복으로 DestroyWindow를 호출하지 않도록 함.
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
#pragma endregion

#pragma region Exception
Window::Exception::Exception(int line, const char* file, HRESULT hr) noexcept
	:
	CustomException(line, file),
	hr(hr)
{}

const char* Window::Exception::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] " << GetErrorCode() << std::endl
		<< "[Description] " << GetErrorString() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Window::Exception::GetType() const noexcept
{
	return "Chili Window Exception";
}

std::string Window::Exception::TranslateErrorCode(HRESULT hr) noexcept
{
	char* pMsgBuf = nullptr;
	// windows will allocate memory for err string and make our pointer point to it
	DWORD nMsgLen = FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr
	);
	// 0 string length returned indicates a failure
	if (nMsgLen == 0)
	{
		return "Unidentified error code";
	}
	// copy error string from windows-allocated buffer to std::string
	std::string errorString = pMsgBuf;
	// free windows buffer
	LocalFree(pMsgBuf);
	return errorString;
}

HRESULT Window::Exception::GetErrorCode() const noexcept
{
	return hr;
}

std::string Window::Exception::GetErrorString() const noexcept
{
	return TranslateErrorCode(hr);
}
#pragma endregion