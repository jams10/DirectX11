#pragma once
#include "WindowsHeader.h" // �� ��� ������ �ֻ�ܿ� include �ؾ� Windows.h ��� ���Ͽ��� �����Ϸ��� �� ��ɵ��� ���ԵǴ� �� ������ �� ����.(��ũ�ΰ� ���� ���Ƿ�)
#include "CustomException.h"
#include "Keyboard.h"

// Window : ������ ���� �� ������ �����.
class Window
{
#pragma region Exception
public:
	class Exception : public CustomException
	{
	public:
		Exception(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept;
		static std::string TranslateErrorCode(HRESULT hr) noexcept;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
	private:
		HRESULT hr;
	};
#pragma endregion
#pragma region WindowClass
private:
	// ������ Ŭ���� ���/������ �����ϴ� �̱���.
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
	Window(int width, int height, const wchar_t* name);
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
public:
	Keyboard kbd;
private:
	int width;
	int height;
	HWND hWnd;
};

// ���� ���� ���� ��ũ��
#define WND_EXCEPT( hr ) Window::Exception( __LINE__,__FILE__,hr )
#define WND_LAST_EXCEPT() Window::Exception( __LINE__,__FILE__,GetLastError() )