#pragma once

#include "WindowsHeader.h"
#include "CustomException.h"
#include <d3d11.h> // �츮�� WindowsHeader���� ��ũ�θ� ���� � API�� ���������� ���� ������ �������ֱ� ���� WindowsHeader ������� ������ include ��.
#include <vector>
#include "DxgiInfoManager.h"

class Graphics
{
#pragma region Exception
public:
	class Exception : public CustomException
	{
		using CustomException::CustomException; // CustomException ������ ���.
	};
	// HrException : HRESULT�� �Ѿ���� ������ ���� ���� ó�� Ŭ����.
	class HrException : public Exception
	{
	public:
		HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs = {}) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
		std::string GetErrorInfo() const noexcept;
	private:
		HRESULT hr;
		std::string info;
	};
	// InfoException : HRESULT�� �������� �ʴ� �Լ����� ���� ó���� ���� ����� ���â�� �ִ� ������ �����ٰ� �Ѱ��ִ� Ŭ����.
	class InfoException : public Exception
	{
	public:
		InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		std::string GetErrorInfo() const noexcept;
	private:
		std::string info;
	};
	// DeviceRemovedException : HRESULT�� DXGI_ERROR_DEVICE_REMOVED�� ��� ���� ó������ ���� Ŭ����.
	class DeviceRemovedException : public HrException
	{
		using HrException::HrException;
	public:
		const char* GetType() const noexcept override;
	private:
		std::string reason;
	};
#pragma endregion
public:
	Graphics(HWND hWnd);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics() = default; // ComPtr�� ����ϰ� �Ǹ鼭 Com ��ü���� �˾Ƽ� Release �ǹǷ� �⺻ �Ҹ��ڷ� �ٲ���.
	void EndFrame();
	void ClearBuffer(float red, float green, float blue) noexcept;
	void DrawTestTriangle(float angle);
private:
#ifndef NDEBUG
	DxgiInfoManager infoManager; // ����� ����� ���� DxgiInfoManager ��ü�� ��� �ְ� ��.
#endif
private:
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
};