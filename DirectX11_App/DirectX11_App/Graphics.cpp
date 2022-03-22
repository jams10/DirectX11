#include "Graphics.h"
#include "dxerr.h"
#include <sstream>
#include <d3dcompiler.h>
#include <cmath>

#pragma comment(lib,"d3d11.lib")        // Direct3D 함수들이 정의된 라이브러리를 링크해줌.
#pragma comment(lib, "D3DCompiler.lib") // 셰이더를 런타임에 컴파일 해줄 때 사용할 수 있지만, 우리는 셰이더를 불러오는 함수를 사용하기 위해 연결해줬음. 

// 그래픽스 관련 예외 검사/던지기 매크로들.
#define GFX_EXCEPT_NOINFO(hr) Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_NOINFO(hrcall) if( FAILED( hr = (hrcall) ) ) throw Graphics::HrException( __LINE__,__FILE__,hr )

#ifndef NDEBUG // 디버그 모드인 경우 추가적인 정보(디버그 출력창의 정보)도 함께 넘겨줌.
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if( FAILED( hr = (hrcall) ) ) throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#define GFX_THROW_INFO_ONLY(call) infoManager.Set(); (call); {auto v = infoManager.GetMessages(); if(!v.empty()) {throw Graphics::InfoException( __LINE__,__FILE__,v);}}
#else
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO_ONLY(call) (call)
#endif

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
}

// 프레임 최종 결과 단계를 의미하는 함수. 프레임 끝에 처리해 줄 것들을 담고 있음.(스왑 체인 Present)
void Graphics::EndFrame()
{
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

// 버퍼를 초기화해주는 함수.
void Graphics::ClearBuffer(float red, float green, float blue) noexcept
{
	const float color[] = { red,green,blue,1.0f };
	pContext->ClearRenderTargetView(pTarget.Get(), color); // 렌더 타겟 뷰를 초기화.
	pTarget.GetAddressOf();
}

void Graphics::DrawTestTriangle(float angle)
{
	// 정점 구조체 생성.
	struct Vertex
	{
		struct
		{
			float x;
			float y;
		} pos;
		struct
		{
			unsigned char r;
			unsigned char g;
			unsigned char b;
			unsigned char a;
		} color;
	};

	// 삼각형을 구성할 정점 배열 생성. 좌표 변환 없이 그냥 곧바로 NDC 좌표로 찍어줌.
	const Vertex vertices[] =
	{
		{ 0.0f,0.5f,255,0,0,0 },
		{ 0.5f,-0.5f,0,255,0,0 },
		{ -0.5f,-0.5f,0,0,255,0 },
		{ -0.3f,0.3f,0,255,0,0 },
		{ 0.3f,0.3f,0,0,255,0 },
		{ 0.0f,-0.8f,255,0,0,0 },
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	D3D11_BUFFER_DESC bd = {}; // 버퍼 서술자.
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = sizeof(vertices);
	bd.StructureByteStride = sizeof(Vertex);

	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = vertices;

	HRESULT hr;
	// 정점 버퍼 생성.
	GFX_THROW_INFO(pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer));
	
	const UINT stride = sizeof(Vertex);
	const UINT offest = 0u;

	// 정점 버퍼 파이프라인에 묶기.
	pContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offest);

	// 인덱스 버퍼 생성
	const unsigned short indices[] =
	{
		0,1,2,
		0,2,3,
		0,4,1,
		2,1,5,
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = sizeof(indices);
	ibd.StructureByteStride = sizeof(unsigned short);
	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = indices;
	GFX_THROW_INFO(pDevice->CreateBuffer(&ibd, &isd, &pIndexBuffer));

	// 인덱스 버퍼 파이프라인에 묶기.
	pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);

	// 변환 행렬을 위한 상수 버퍼 생성.
	struct ConstantBuffer
	{
		struct
		{
			float element[4][4];
		} transformation;
	};
	const ConstantBuffer cb =
	{
		{
			(3.0f / 4.0f) * std::cos(angle),	std::sin(angle),	0.0f,	0.0f,
			(3.0f / 4.0f) * -std::sin(angle),	std::cos(angle),	0.0f,	0.0f,
			0.0f,								0.0f,				1.0f,	0.0f,
			0.0f,								0.0f,				0.0f,	1.0f,
		}
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
	D3D11_BUFFER_DESC cbd;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;             //  우리가 matrix를 매 프레임마다 업데이트 해줄것이기 때문에 DYNAMIC으로 설정.
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // CPU에서 값을 계속해서 행렬 값을 업데이트 해줄 것이기 때문에 CPU_ACCESS_WRITE로 설정.
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = sizeof(cb);
	cbd.StructureByteStride = 0u;                // 우리가 vertex buffer나 index buffer 처럼 배열로 구성된게 아니기 때문에 0u로 설정.
	D3D11_SUBRESOURCE_DATA csd = {};
	csd.pSysMem = &cb;
	GFX_THROW_INFO(pDevice->CreateBuffer(&cbd, &csd, &pConstantBuffer));

	// 픽셀 셰이더에 상수 버퍼 묶기.
	pContext->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());

	// 픽셀 셰이더 생성.
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	GFX_THROW_INFO(D3DReadFileToBlob(L"PixelShader.cso", &pBlob));
	GFX_THROW_INFO(pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader));

	// 픽셀 셰이더 파이프라인에 묶기.
	pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);

	// 정점 셰이더 생성.
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	GFX_THROW_INFO(D3DReadFileToBlob(L"VertexShader.cso", &pBlob)); // 셰이더의 바이트 코드를 불러옴.
	GFX_THROW_INFO(pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader));

	// 정점 셰이더 파이프라인에 묶기.
	pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);

	// 입력 레이아웃 서술자.
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 8u, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	// 입력 레이아웃 생성.
	GFX_THROW_INFO(pDevice->CreateInputLayout(
		ied, (UINT)std::size(ied),
		pBlob->GetBufferPointer(), // 정점 셰이더의 바이트 코드를 넘겨줌.
		pBlob->GetBufferSize(),
		&pInputLayout
	));

	// 입력 레이아웃 파이프라인에 묶기.
	pContext->IASetInputLayout(pInputLayout.Get());

	// 렌더 타겟 파이프라인에 묶기.
	pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), nullptr);

	// 기본 도형을 triangle list로 설정.
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 뷰포트 설정
	D3D11_VIEWPORT vp;
	vp.Width = 800;
	vp.Height = 600;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0; // 렌더 타겟의 뷰포트 위치.
	vp.TopLeftY = 0;
	pContext->RSSetViewports(1u, &vp);

	GFX_THROW_INFO_ONLY(pContext->DrawIndexed((UINT)std::size(indices), 0u, 0u));
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