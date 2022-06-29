#include "Graphics.h"
#include "../ErrorHandling/dxerr.h"
#include <sstream>
#include <cmath>
#include <DirectXMath.h>
#include "../ErrorHandling/GraphicsThrowMacros.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "DepthStencil.h"

#pragma comment(lib,"d3d11.lib")        // Direct3D 함수들이 정의된 라이브러리를 링크해줌.
#pragma comment(lib, "D3DCompiler.lib") // 셰이더를 런타임에 컴파일 해줄 때 사용할 수 있지만, 우리는 셰이더를 불러오는 함수를 사용하기 위해 연결해줬음. 

Graphics::Graphics(HWND hWnd, int width, int height)
	:
	width(width),
	height(height)
{
	// 스왑 체인의 설정 정보를 담은 구조체
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = 0;  // backbuffer 너비 0이면, 런타임에 알아서 출력 윈도우에 크기에 따라 잡아줌.
	sd.BufferDesc.Height = 0; // backbuffer 높이
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // 픽셀 형식
	sd.BufferDesc.RefreshRate.Numerator = 0;           // hz의 refresh rate 분자
	sd.BufferDesc.RefreshRate.Denominator = 0;         // hz의 refresh rate 분모
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;                 // 이미지가 주어진 모니터의 해상도에 맞추기 위해 어떻게 늘여지는지에 대한 방법을 가리킴.
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // surface에 이미지를 생성하기 위해 raster가 사용하는 방법을 가리킴.
	sd.SampleDesc.Count = 1;                           // 다중 표본화 개수
	sd.SampleDesc.Quality = 0;                         // 다중 표본화 품질
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;  // 버퍼를 렌더타겟 용도로 사용.
	sd.BufferCount = 1;                                // 1개의 back buffer를 사용. 그러면 front/back 으로 총 2개의 버퍼를 사용.
	sd.OutputWindow = hWnd;                            // 출력 윈도우를 지정.
	sd.Windowed = TRUE;                                // 창모드를 적용.
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;          // IDXGISwapChain1::Present1 호출 후에 디스플레이 화면에 있는 픽셀들을 어떻게 처리할 것인가? 여기서는 그냥 버림.
	sd.Flags = 0;                                      // 추가적인 플래그들.

	UINT swapCreateFlags = 0u;
#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;      // 디버그 모드인 경우에 디바이스 생성 플래그를 D3D11_CREATE_DEVICE_DEBUG로 설정해줌.
#endif

	HRESULT hr; // API 함수가 리턴하는 HRESULT를 받아와 저장해 두기 위함.

	// device와 swap chain, device context를 생성함.
	GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
		nullptr,                  // IDXGIAdapter를 nullptr로 설정해, 기본 어댑터를 선택해줌.
		D3D_DRIVER_TYPE_HARDWARE, // 생성할 드라이버 타입.
		nullptr,                  // 소프트웨어 래스터라이저를 구현하는 DLL에 대한 핸들.
		swapCreateFlags,          // 디버그 모드의 경우 D3D11_CREATE_DEVICE_DEBUG를 설정해, 디버깅 분석 정보를 얻을 수 있음.
		nullptr,                  // D3D_FEATURE_LEVEL. D3D 장치 피쳐 레벨. nullptr로 주면, 기본 6가지 피쳐 레벨을 사용.
		0,                        // pFeatureLevels에 있는 원소들의 개수.
		D3D11_SDK_VERSION,        // SDK_VERSION. 피쳐 레벨과는 다름.
		&sd,                      // 스왑 체인 서술자
		&pSwap,                   // 생성된 스왑 체인
		&pDevice,                 // 생성된 디바이스
		nullptr,                  // *pFeatureLevel. 가능한 피쳐레벨을 넣어줌. 어떤 피쳐 레벨이 지원되는지 알 필요 없으면 그냥 nullptr.
		&pContext                 // 생성된 디바이스 컨텍스트
	));

	// 스왑 체인 내부에 들고 있는 텍스쳐 자원(back buffer)에 대한 접근을 얻어옴.
	Microsoft::WRL::ComPtr<ID3D11Resource> pBackBuffer;
	GFX_THROW_INFO(pSwap->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));
	GFX_THROW_INFO(pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pTarget));

	// imgui d3d 구현을 초기화
	ImGui_ImplDX11_Init(pDevice.Get(), pContext.Get());
}

// 렌더링 작업을 위한 사전 준비 작업을 처리해주는 함수.
void Graphics::BeginFrame(float red, float green, float blue) noexcept
{
	// imgui가 사용이 허용된 경우, 새 프레임을 렌더링 하기 위한 사전 작업 수행.
	if (imguiEnabled)
	{
		ImGui_ImplDX11_NewFrame(); // 새 프레임을 그려주기 전에 호출해 주어야 함.
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();         // 새 프레임 만들어줌.
	}

	const float color[] = { red,green,blue,0.0f };
	pContext->ClearRenderTargetView(pTarget.Get(), color);
}

// 프레임 최종 결과 단계를 의미하는 함수. 프레임 끝에 처리해 줄 것들을 담고 있음.(스왑 체인 Present)
void Graphics::EndFrame()
{
	// imgui가 사용이 허용된 경우, imgui를 화면에 렌더링 해줌.
	if (imguiEnabled)
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	HRESULT hr;
#ifndef NDEBUG
	infoManager.Set(); // DXGI_ERROR_DEVICE_REMOVED는 GetDeviceRemovedReason() 함수로 그 이유를 알 수 있음.
	                   // 이 이유에 관한 추가적인 정보를 메시지 박스로 넘겨받기 위해 이 시점에서 Set()을 해서
					   // 꺼내올 에러 메시지 큐 인덱스를 이동 시킨 다음에 아래에서 에러가 발생하면 해당 에러에 대해서만 받아볼 수 있도록 함.
#endif
	if (FAILED(hr = pSwap->Present(1u, 0u)))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED) // DXGI_ERROR_DEVICE_REMOVED 에러인 경우에 따로 처리해줌.
		{
			throw GFX_DEVICE_REMOVED_EXCEPT(pDevice->GetDeviceRemovedReason());
		}
		else
		{
			throw GFX_EXCEPT(hr);
		}
	}
}

void Graphics::BindSwapBuffer() noexcept
{
	pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), nullptr);

	// configure viewport
	D3D11_VIEWPORT vp;
	vp.Width = (float)width;
	vp.Height = (float)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	pContext->RSSetViewports(1u, &vp);
}

void Graphics::BindSwapBuffer(const DepthStencil& ds) noexcept
{
	pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), ds.pDepthStencilView.Get());

	// configure viewport
	D3D11_VIEWPORT vp;
	vp.Width = (float)width;
	vp.Height = (float)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	pContext->RSSetViewports(1u, &vp);
}

void Graphics::DrawIndexed(UINT count) noxnd
{
	GFX_THROW_INFO_ONLY(pContext->DrawIndexed(count, 0u, 0u));
}

void Graphics::SetProjection(DirectX::FXMMATRIX proj) noexcept
{
	projection = proj;
}

DirectX::XMMATRIX Graphics::GetProjection() const noexcept
{
	return projection;
}

void Graphics::SetCamera(DirectX::FXMMATRIX cam) noexcept
{
	camera = cam;
}

DirectX::XMMATRIX Graphics::GetCamera() const noexcept
{
	return camera;
}

//void Graphics::ResizeWindow()
//{
//	if (pSwap)
//	{
//		pContext->OMSetRenderTargets(0, 0, 0);
//
//		// 스왑 체인의 버퍼들에 대한 모든 참조를 해제함.
//		pTarget.Reset(); // ComPtr은 nullptr 초기화 혹은 Reset을 통해 COM 객체를 release 할 수 있음.
//		pDSV.Reset();
//
//		HRESULT hr;
//
//		// 존재 하는 버퍼 카운트와 포맷을 보존함.
//		// 윈도우 핸들에 대한 클라이언트 영역 크기에 맞는 높이와 너비를 자동으로 선택해줌.
//		GFX_THROW_INFO(pSwap->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0));
//
//		// 스왑 체인 내부의 버퍼를 얻어오고, 렌더 타겟 뷰를 만들어줌.
//		ID3D11Texture2D* pBuffer;
//		GFX_THROW_INFO(pSwap->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBuffer));
//		GFX_THROW_INFO(pDevice->CreateRenderTargetView(pBuffer, NULL, &pTarget));
//		
//		pBuffer->Release(); // COM 객체를 직접 Release.
//
//		RECT clientRect;
//		GetClientRect(hWnd, &clientRect);
//
//		UINT width = clientRect.right - clientRect.left;
//		UINT height = clientRect.bottom - clientRect.top;
//
//		// 깊이 버퍼도 만들어줌. 깊이 스텐실 state의 경우 변경 사항이 없고 파이프라인에서 해제 해줄 필요 없기 때문에,
//		// 텍스쳐 자원과 깊이 스텐실 뷰만 새로 생성해서 다시 파이프라인에 묶어줌.
//		// 깊이 스텐실용 텍스쳐 생성.
//		Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencil;
//		D3D11_TEXTURE2D_DESC descDepth = {};
//		descDepth.Width = width;                // 새로 수정된 윈도우 크기(
//		descDepth.Height = height;
//		descDepth.MipLevels = 1u;
//		descDepth.ArraySize = 1u;
//		descDepth.Format = DXGI_FORMAT_D32_FLOAT;
//		descDepth.SampleDesc.Count = 1u;
//		descDepth.SampleDesc.Quality = 0u;
//		descDepth.Usage = D3D11_USAGE_DEFAULT;
//		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
//		GFX_THROW_INFO(pDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencil));
//
//		// 깊이 스텐실 텍스쳐에 대한 뷰 생성.
//		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
//		descDSV.Format = DXGI_FORMAT_D32_FLOAT;
//		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
//		descDSV.Texture2D.MipSlice = 0u;
//		GFX_THROW_INFO(pDevice->CreateDepthStencilView(
//			pDepthStencil.Get(), &descDSV, &pDSV
//		));
//
//		// 렌더 타겟 뷰와 깊이 버퍼를 출력 병합기에 묶어줌.
//		pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), pDSV.Get());
//
//		// 뷰포트 재설정.
//		D3D11_VIEWPORT vp;
//		vp.Width = width;
//		vp.Height = height;
//		vp.MinDepth = 0.0f;
//		vp.MaxDepth = 1.0f;
//		vp.TopLeftX = 0;
//		vp.TopLeftY = 0;
//		pContext->RSSetViewports(1u, &vp);
//	}
//}

UINT Graphics::GetWidth() const noexcept
{
	return width;
}

UINT Graphics::GetHeight() const noexcept
{
	return height;
}


void Graphics::EnableImgui() noexcept
{
	imguiEnabled = true;
}

void Graphics::DisableImgui() noexcept
{
	imguiEnabled = false;
}

bool Graphics::IsImguiEnabled() const noexcept
{
	return imguiEnabled;
}

#pragma region Exception
Graphics::HrException::HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs) noexcept
	:
	Exception(line, file),
	hr(hr)
{
	for (const auto& m : infoMsgs)
	{
		info += m;
		info.push_back('\n');
	}
	// 마지막에 개행 문자가 있는 경우 삭제해줌.
	if (!info.empty())
	{
		info.pop_back();
	}
}

// what() : 출력할 에러 문자열을 생성해 리턴하는 함수. std::exception으로 부터 오버라이딩.
const char* Graphics::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Error String] " << GetErrorString() << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl;
	
	if (!info.empty()) // 디버그 출력창의 추가적인 정보 또한 포함시켜서 넘겨주도록 함.
	{
		oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	}
	
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

// 예외 타입 문자열을 리턴해주는 함수.
const char* Graphics::HrException::GetType() const noexcept
{
	return "Custom Graphics Exception";
}

// 에러 코드(HRESULT)를 리턴해주는 함수.
HRESULT Graphics::HrException::GetErrorCode() const noexcept
{
	return hr;
}

// dxerr 라이브러리를 통해 에러 문자열을 리턴해주는 함수.
std::string Graphics::HrException::GetErrorString() const noexcept
{
	return DXGetErrorStringA(hr);
}

// dxerr 라이브러리를 통해 에러에 관한 설명 문자열을 리턴해주는 함수.
std::string Graphics::HrException::GetErrorDescription() const noexcept
{
	char buf[512];
	DXGetErrorDescriptionA(hr, buf, sizeof(buf));
	return buf;
}

// 디버그 출력 창 에러 정보 문자열을 리턴하는 함수.
std::string Graphics::HrException::GetErrorInfo() const noexcept
{
	return info;
}

// 예외 타입 문자열을 리턴해주는 함수.
const char* Graphics::DeviceRemovedException::GetType() const noexcept
{
	return "Custom Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}

Graphics::InfoException::InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept
	:
	Exception(line, file)
{
	// join all info messages with newlines into single string
	for (const auto& m : infoMsgs)
	{
		info += m;
		info.push_back('\n');
	}
	// remove final newline if exists
	if (!info.empty())
	{
		info.pop_back();
	}
}

// what() : 출력할 에러 문자열을 생성해 리턴하는 함수. std::exception으로 부터 오버라이딩.
const char* Graphics::InfoException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

// 예외 타입 문자열을 리턴해주는 함수.
const char* Graphics::InfoException::GetType() const noexcept
{
	return "Custom Graphics Info Exception";
}

// 디버그 출력 창 에러 정보 문자열을 리턴하는 함수.
std::string Graphics::InfoException::GetErrorInfo() const noexcept
{
	return info;
}
#pragma endregion