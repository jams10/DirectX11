#pragma once
#include "WindowsHeader.h" // �� ��� ������ �ֻ�ܿ� include �ؾ� Windows.h ��� ���Ͽ��� �����Ϸ��� �� ��ɵ��� ���ԵǴ� �� ������ �� ����.(��ũ�ΰ� ���� ���Ƿ�)
#include "CustomException.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"
#include <optional>
#include <memory>

// Window : ������ ���� �� ������ �����.
class Window
{
#pragma region Exception
public:
	class Exception : public CustomException
	{
		using CustomException::CustomException;
	public:
		static std::string TranslateErrorCode(HRESULT hr) noexcept;
	};
	// HrException : HRESULT�� �Ѿ���� ������ ���� ���� ó�� Ŭ����.
	class HrException : public Exception
	{
	public:
		HrException(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorDescription() const noexcept;
	private:
		HRESULT hr;
	};
	// NoGfxException : Graphics ��ü�� nullptr�� ��츦 ���� ó������ ���� Ŭ����.
	class NoGfxException : public Exception
	{
	public:
		using Exception::Exception;
		const char* GetType() const noexcept override;
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
	void SetTitle(const std::wstring& title);
	static std::optional<int> ProcessMessages() noexcept; // ��� �����쿡 ���� �޽����� ó���ؾ� �ϹǷ� static���� ������.
	Graphics& Gfx();
private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
public:
	Keyboard kbd;
	Mouse mouse;
private:
	int width;
	int height;
	HWND hWnd;
	std::unique_ptr<Graphics> pGfx;
};

// ���� ���� ���� ��ũ��
#define WND_EXCEPT( hr ) Window::HrException( __LINE__,__FILE__,(hr) )
#define WND_LAST_EXCEPT() Window::HrException( __LINE__,__FILE__,GetLastError() )
#define WND_NOGFX_EXCEPT() Window::NoGfxException( __LINE__,__FILE__ ) 