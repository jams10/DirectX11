#pragma once

#include "../Windows/WindowsHeader.h"
#include "../ErrorHandling/CustomException.h"
#include <d3d11.h> // �츮�� WindowsHeader���� ��ũ�θ� ���� � API�� ���������� ���� ������ �������ֱ� ���� WindowsHeader ������� ������ include ��.
#include <vector>
#include <memory>
#include <random>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include "../ErrorHandling/DxgiInfoManager.h"
#include "ConditionalNoexcept.h"

class DepthStencil;

namespace Bind
{
	class Bindable;
}

class Graphics
{
	friend class GraphicsResource;
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
	Graphics(HWND hWnd, int width, int height);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics() = default; // ComPtr�� ����ϰ� �Ǹ鼭 Com ��ü���� �˾Ƽ� Release �ǹǷ� �⺻ �Ҹ��ڷ� �ٲ���.
	void BeginFrame(float red, float green, float blue) noexcept;
	void EndFrame();
	void BindSwapBuffer() noexcept;
	void BindSwapBuffer(const DepthStencil& ds) noexcept;
	void DrawIndexed(UINT count) noxnd;
	void SetProjection(DirectX::FXMMATRIX proj) noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;
	void SetCamera(DirectX::FXMMATRIX cam) noexcept;
	DirectX::XMMATRIX GetCamera() const noexcept;

	void ResizeWindow();

	// Imgui
	void EnableImgui() noexcept;
	void DisableImgui() noexcept;
	bool IsImguiEnabled() const noexcept;
	UINT GetWidth() const noexcept;
	UINT GetHeight() const noexcept;
private:
	UINT width;
	UINT height;
	DirectX::XMMATRIX projection; // ���� ���
	DirectX::XMMATRIX camera;     // ��(ī�޶�) ���
	bool imguiEnabled = true;     // imgui ��� ��� ����.
	HWND hWnd;
private:
#ifndef NDEBUG
	DxgiInfoManager infoManager; // ����� ����� ���� DxgiInfoManager ��ü�� ��� �ְ� ��.
#endif
private:
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSV;
};