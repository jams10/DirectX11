#pragma once

#include "WindowsHeader.h"
#include <d3d11.h> // 우리가 WindowsHeader에서 매크로를 통해 어떤 API를 제외할지에 대한 설정을 적용해주기 위해 WindowsHeader 헤더파일 다음에 include 함.

class Graphics
{
public:
	Graphics(HWND hWnd);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics();
	void EndFrame();
	void ClearBuffer(float red, float green, float blue) noexcept;
private:
	ID3D11Device* pDevice = nullptr;
	IDXGISwapChain* pSwap = nullptr;
	ID3D11DeviceContext* pContext = nullptr;
	ID3D11RenderTargetView* pTarget = nullptr;
};