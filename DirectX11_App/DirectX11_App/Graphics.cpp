#include "Graphics.h"
#include "dxerr.h"
#include <sstream>

#pragma comment(lib,"d3d11.lib") // Direct3D 함수들이 정의된 라이브러리를 링크해줌.

#define GFX_THROW_FAILED(hrcall) if( FAILED( hr = (hrcall) ) ) throw Graphics::HrException( __LINE__,__FILE__,hr )
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr) )

Graphics::Graphics(HWND hWnd)
{
	// 스왑 체인의 설정 정보를 담은 구조체
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = 0;						   // backbuffer 너비
	sd.BufferDesc.Height = 0;						   // backbuffer 높이
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

	HRESULT hr; // API 함수가 리턴하는 HRESULT를 받아와 저장해 두기 위함.

	// device와 swap chain, device context를 생성함.
	GFX_THROW_FAILED(D3D11CreateDeviceAndSwapChain(
		nullptr,                  // IDXGIAdapter를 nullptr로 설정해, 기본 어댑터를 선택해줌.
		D3D_DRIVER_TYPE_HARDWARE, // 생성할 드라이버 타입.
		nullptr,                  // 소프트웨어 래스터라이저를 구현하는 DLL에 대한 핸들.
		0,                        // 디버그 모드의 경우 D3D11_CREATE_DEVICE_DEBUG를 설정해, 디버깅 분석 정보를 얻을 수 있음.
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
	ID3D11Resource* pBackBuffer = nullptr;
	GFX_THROW_FAILED(pSwap->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&pBackBuffer)));
	GFX_THROW_FAILED(pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pTarget));
	pBackBuffer->Release();
}

Graphics::~Graphics()
{
	// 객체들의 Release를 호출 해 레퍼런스 카운트를 줄여줌.	
	if (pTarget != nullptr)
	{
		pTarget->Release();
	}
	if (pContext != nullptr)
	{
		pContext->Release();
	}
	if (pSwap != nullptr)
	{
		pSwap->Release();
	}
	if (pDevice != nullptr)
	{
		pDevice->Release();
	}
}

// 프레임 최종 결과 단계를 의미하는 함수. 프레임 끝에 처리해 줄 것들을 담고 있음.(스왑 체인 Present)
void Graphics::EndFrame()
{
	HRESULT hr;
	if (FAILED(hr = pSwap->Present(1u, 0u)))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED) // DXGI_ERROR_DEVICE_REMOVED 에러인 경우에 따로 처리해줌.
		{
			throw GFX_DEVICE_REMOVED_EXCEPT(pDevice->GetDeviceRemovedReason());
		}
		else
		{
			GFX_THROW_FAILED(hr);
		}
	}
}

// 버퍼를 초기화해주는 함수.
void Graphics::ClearBuffer(float red, float green, float blue) noexcept
{
	const float color[] = { red,green,blue,1.0f };
	pContext->ClearRenderTargetView(pTarget, color); // 렌더 타겟 뷰를 초기화.
}

#pragma region Exception
Graphics::HrException::HrException(int line, const char* file, HRESULT hr) noexcept
	:
	Exception(line, file),
	hr(hr)
{}

// what() : 출력할 에러 문자열을 생성해 리턴하는 함수. std::exception으로 부터 오버라이딩.
const char* Graphics::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Error String] " << GetErrorString() << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl
		<< GetOriginString();
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

// 예외 타입 문자열을 리턴해주는 함수.
const char* Graphics::DeviceRemovedException::GetType() const noexcept
{
	return "Custom Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}
#pragma endregion