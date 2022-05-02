#pragma once
#include "WindowsHeader.h" // �� ��� ������ �ֻ�ܿ� include �ؾ� Windows.h ��� ���Ͽ��� �����Ϸ��� �� ��ɵ��� ���ԵǴ� �� ������ �� ����.(��ũ�ΰ� ���� ���Ƿ�)
#include "../ErrorHandling/CustomException.h"
#include "../Input/Keyboard.h"
#include "../Input/Mouse.h"
#include "../Graphics.h"
#include <optional>
#include <memory>
#include "../ErrorHandling/WindowsThrowMacros.h"

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
	void EnableCursor() noexcept;
	void DisableCursor() noexcept;
	static std::optional<int> ProcessMessages() noexcept; // ��� �����쿡 ���� �޽����� ó���ؾ� �ϹǷ� static���� ������.
	std::pair<UINT, UINT> GetWindowSize() noexcept;

	Graphics& Gfx();
private:
	void ConfineCursor() noexcept;
	void FreeCursor() noexcept;
	void HideCursor() noexcept;
	void ShowCursor() noexcept;
	void EnableImGuiMouse() noexcept;
	void DisableImGuiMouse() noexcept;
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
public:
	Keyboard kbd;
	Mouse mouse;
private:
	bool cursorEnabled = true;
	int width;
	int height;	
	HWND hWnd;
	std::unique_ptr<Graphics> pGfx;
	std::vector<BYTE> rawBuffer;
};

