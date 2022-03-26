#pragma once

#include "../Windows/WindowsHeader.h"
#include "../ErrorHandling/CustomException.h"
#include <d3d11.h> // 우리가 WindowsHeader에서 매크로를 통해 어떤 API를 제외할지에 대한 설정을 적용해주기 위해 WindowsHeader 헤더파일 다음에 include 함.
#include <vector>
#include <memory>
#include <random>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include "../ErrorHandling/DxgiInfoManager.h"

class Graphics
{
	friend class Bindable;
#pragma region Exception
public:
	class Exception : public CustomException
	{
		using CustomException::CustomException; // CustomException 생성자 상속.
	};
	// HrException : HRESULT로 넘어오는 에러에 대한 예외 처리 클래스.
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
	// InfoException : HRESULT를 리턴하지 않는 함수들의 예외 처리를 위해 디버그 출력창에 있는 정보만 가져다가 넘겨주는 클래스.
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
	// DeviceRemovedException : HRESULT가 DXGI_ERROR_DEVICE_REMOVED인 경우 따로 처리해줄 예외 클래스.
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
	~Graphics() = default; // ComPtr을 사용하게 되면서 Com 객체들이 알아서 Release 되므로 기본 소멸자로 바꿔줌.
	void BeginFrame(float red, float green, float blue) noexcept;
	void EndFrame();
	void DrawIndexed(UINT count) noexcept(!IS_DEBUG);
	void SetProjection(DirectX::FXMMATRIX proj) noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;
	void SetCamera(DirectX::FXMMATRIX cam) noexcept;
	DirectX::XMMATRIX GetCamera() const noexcept;

	void ResizeWindow();

	// Imgui
	void EnableImgui() noexcept;
	void DisableImgui() noexcept;
	bool IsImguiEnabled() const noexcept;
private:
	DirectX::XMMATRIX projection; // 투영 행렬
	DirectX::XMMATRIX camera;     // 뷰(카메라) 행렬
	bool imguiEnabled = true;     // imgui 사용 허용 여부.
	HWND hWnd;
private:
#ifndef NDEBUG
	DxgiInfoManager infoManager; // 디버그 모드일 때만 DxgiInfoManager 객체를 들고 있게 함.
#endif
private:
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSV;
};