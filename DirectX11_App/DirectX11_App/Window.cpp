#include "Window.h"
#include <sstream>
#include "resource.h" // 아이콘을 비롯 여러 자원들을 얻어오기 위함.

#pragma region WindowClass
// 윈도우 클래스
Window::WindowClass Window::WindowClass::wndClass; // static 멤버 변수 정의
// WindowClass 생성자. 윈도우 클래스 생성을 담당.
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
// WindowClass 소멸자. 등록한 윈도우 해제 담당.
Window::WindowClass::~WindowClass()
{
	UnregisterClass(wndClassName, GetInstance()); // 등록한 윈도우를 해제 해줌.
}
// 윈도우 클래스의 이름을 리턴해주는 함수.
const wchar_t* Window::WindowClass::GetName() noexcept
{
	return wndClassName;
}
// 프로그램 인스턴스에 대한 핸들을 리턴해주는 함수.
HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInst;
}
#pragma endregion

#pragma region Window
// Window 클래스 생성자. 윈도우 생성 및 설정
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
// Window 클래스 소멸자. 생성한 윈도우 파괴.
Window::~Window()
{
	DestroyWindow(hWnd);
}
// 직접 만들어준 멤버 함수를 윈도우 프로시져로 사용하기 위한 기본 설정을 담당하는 함수.
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
// 윈도우 메시지를 우리가 작성한 멤버 함수로 전달하기 위한 함수.
LRESULT CALLBACK Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// window class에 대한 포인터를 얻어옴.
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	
	// window class 핸들러에 메시지를 전달함.
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}
// 실제 윈도우 메시지를 처리해 줄 함수.
LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);                    	 
		return 0;
	case WM_KILLFOCUS:    // 윈도우에 대한 포커스를 잃어버리면 키 입력 상태를 모두 초기화 해줌.
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
// what() : 출력할 에러 문자열을 생성해 리턴하는 함수.
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
// 예외 타입을 문자열로 리턴하는 함수.
const char* Window::Exception::GetType() const noexcept
{
	return "Custom Window Exception";
}

// 에러 코드를 문자열로 변환해주는 함수.
std::string Window::Exception::TranslateErrorCode(HRESULT hr) noexcept
{
	char* pMsgBuf = nullptr;
   /*
		<FormatMessage 함수>
		HRESULT 값을 받아서 에러 코드를 에러 메시지로 변환해줌.
		윈도우가 에러 문자열을 위한 메모리를 할당하고, 인자로 넘긴 포인터가 이를 가리키도록 해줌.
	    FORMAT_MESSAGE_ALLOCATE_BUFFER : os가 알아서 메시지 저장 공간을 할당하도록 함.
		FORMAT_MESSAGE_FROM_SYSTEM     : os로 부터 에러 메시지 가져옴.
		FORMAT_MESSAGE_IGNORE_INSERTS  : 메시지 정의 삽입 시퀀스 무시.
	*/
	const DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPWSTR>(&pMsgBuf), 0, nullptr
	);
	// 문자열 길이가 0이면 실패를 나타냄.
	if (nMsgLen == 0)
	{
		return "Unidentified error code";
	}
	// 에러 문자열을 윈도우가 할당해준 버퍼에서 std::string으로 복사해줌.
	std::string errorString = pMsgBuf;
	// free windows buffer
	/*
		<LocalFree 함수>
		특정 로컬 메모리 오브젝트를 해제하고, 그 핸들을 유효하지 않도록 바꿈.
		에러 메시지를 위해 할당한 공간을 해제.
	*/
	LocalFree(pMsgBuf);
	return errorString;
}
// 에러 코드를 반환해주는 함수.
HRESULT Window::Exception::GetErrorCode() const noexcept
{
	return hr;
}
// 에러 코드에 대한 에러 문자열을 반환해주는 함수.
std::string Window::Exception::GetErrorString() const noexcept
{
	return TranslateErrorCode(hr);
}
#pragma endregion