#pragma once
#include "WindowsHeader.h" // �� ��� ������ �ֻ�ܿ� include �ؾ� Windows.h ��� ���Ͽ��� �����Ϸ��� �� ��ɵ��� ���ԵǴ� �� ������ �� ����.(��ũ�ΰ� ���� ���Ƿ�)

// Window : ������ ���� �� ������ �����.
class Window
{
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