#pragma once
#include "WindowsHeader.h" // 이 헤더 파일을 최상단에 include 해야 Windows.h 헤더 파일에서 제외하려고 한 기능들이 포함되는 걸 방지할 수 있음.(매크로가 먼저 들어가므로)

// Window : 윈도우 생성 및 해제를 담당함.
class Window
{
#pragma region WindowClass
private:
	// 윈도우 클래스 등록/해제를 관리하는 싱글톤.
	class WindowClass
	{
	public:
		static const wchar_t* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;
	private:
		WindowClass() noexcept;
		~WindowClass();
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
		
		static constexpr const wchar_t* wndClassName = L"Chili Direct3D Engine Window";
		static WindowClass wndClass; // SINGLETON
		HINSTANCE hInst;
	};
#pragma endregion
public:
	Window(int width, int height, const wchar_t* name) noexcept;
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
private:
	int width;
	int height;
	HWND hWnd;
};