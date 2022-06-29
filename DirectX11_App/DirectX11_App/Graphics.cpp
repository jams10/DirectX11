#include "Graphics.h"
#include "../ErrorHandling/dxerr.h"
#include <sstream>
#include <cmath>
#include <DirectXMath.h>
#include "../ErrorHandling/GraphicsThrowMacros.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "DepthStencil.h"

#pragma comment(lib,"d3d11.lib")        // Direct3D �Լ����� ���ǵ� ���̺귯���� ��ũ����.
#pragma comment(lib, "D3DCompiler.lib") // ���̴��� ��Ÿ�ӿ� ������ ���� �� ����� �� ������, �츮�� ���̴��� �ҷ����� �Լ��� ����ϱ� ���� ����������. 

Graphics::Graphics(HWND hWnd, int width, int height)
	:
	width(width),
	height(height)
{
	// ���� ü���� ���� ������ ���� ����ü
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = 0;  // backbuffer �ʺ� 0�̸�, ��Ÿ�ӿ� �˾Ƽ� ��� �����쿡 ũ�⿡ ���� �����.
	sd.BufferDesc.Height = 0; // backbuffer ����
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // �ȼ� ����
	sd.BufferDesc.RefreshRate.Numerator = 0;           // hz�� refresh rate ����
	sd.BufferDesc.RefreshRate.Denominator = 0;         // hz�� refresh rate �и�
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;                 // �̹����� �־��� ������� �ػ󵵿� ���߱� ���� ��� �ÿ��������� ���� ����� ����Ŵ.
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // surface�� �̹����� �����ϱ� ���� raster�� ����ϴ� ����� ����Ŵ.
	sd.SampleDesc.Count = 1;                           // ���� ǥ��ȭ ����
	sd.SampleDesc.Quality = 0;                         // ���� ǥ��ȭ ǰ��
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;  // ���۸� ����Ÿ�� �뵵�� ���.
	sd.BufferCount = 1;                                // 1���� back buffer�� ���. �׷��� front/back ���� �� 2���� ���۸� ���.
	sd.OutputWindow = hWnd;                            // ��� �����츦 ����.
	sd.Windowed = TRUE;                                // â��带 ����.
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;          // IDXGISwapChain1::Present1 ȣ�� �Ŀ� ���÷��� ȭ�鿡 �ִ� �ȼ����� ��� ó���� ���ΰ�? ���⼭�� �׳� ����.
	sd.Flags = 0;                                      // �߰����� �÷��׵�.

	UINT swapCreateFlags = 0u;
#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;      // ����� ����� ��쿡 ����̽� ���� �÷��׸� D3D11_CREATE_DEVICE_DEBUG�� ��������.
#endif

	HRESULT hr; // API �Լ��� �����ϴ� HRESULT�� �޾ƿ� ������ �α� ����.

	// device�� swap chain, device context�� ������.
	GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
		nullptr,                  // IDXGIAdapter�� nullptr�� ������, �⺻ ����͸� ��������.
		D3D_DRIVER_TYPE_HARDWARE, // ������ ����̹� Ÿ��.
		nullptr,                  // ����Ʈ���� �����Ͷ������� �����ϴ� DLL�� ���� �ڵ�.
		swapCreateFlags,          // ����� ����� ��� D3D11_CREATE_DEVICE_DEBUG�� ������, ����� �м� ������ ���� �� ����.
		nullptr,                  // D3D_FEATURE_LEVEL. D3D ��ġ ���� ����. nullptr�� �ָ�, �⺻ 6���� ���� ������ ���.
		0,                        // pFeatureLevels�� �ִ� ���ҵ��� ����.
		D3D11_SDK_VERSION,        // SDK_VERSION. ���� �������� �ٸ�.
		&sd,                      // ���� ü�� ������
		&pSwap,                   // ������ ���� ü��
		&pDevice,                 // ������ ����̽�
		nullptr,                  // *pFeatureLevel. ������ ���ķ����� �־���. � ���� ������ �����Ǵ��� �� �ʿ� ������ �׳� nullptr.
		&pContext                 // ������ ����̽� ���ؽ�Ʈ
	));

	// ���� ü�� ���ο� ��� �ִ� �ؽ��� �ڿ�(back buffer)�� ���� ������ ����.
	Microsoft::WRL::ComPtr<ID3D11Resource> pBackBuffer;
	GFX_THROW_INFO(pSwap->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));
	GFX_THROW_INFO(pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pTarget));

	// imgui d3d ������ �ʱ�ȭ
	ImGui_ImplDX11_Init(pDevice.Get(), pContext.Get());
}

// ������ �۾��� ���� ���� �غ� �۾��� ó�����ִ� �Լ�.
void Graphics::BeginFrame(float red, float green, float blue) noexcept
{
	// imgui�� ����� ���� ���, �� �������� ������ �ϱ� ���� ���� �۾� ����.
	if (imguiEnabled)
	{
		ImGui_ImplDX11_NewFrame(); // �� �������� �׷��ֱ� ���� ȣ���� �־�� ��.
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();         // �� ������ �������.
	}

	const float color[] = { red,green,blue,0.0f };
	pContext->ClearRenderTargetView(pTarget.Get(), color);
}

// ������ ���� ��� �ܰ踦 �ǹ��ϴ� �Լ�. ������ ���� ó���� �� �͵��� ��� ����.(���� ü�� Present)
void Graphics::EndFrame()
{
	// imgui�� ����� ���� ���, imgui�� ȭ�鿡 ������ ����.
	if (imguiEnabled)
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	HRESULT hr;
#ifndef NDEBUG
	infoManager.Set(); // DXGI_ERROR_DEVICE_REMOVED�� GetDeviceRemovedReason() �Լ��� �� ������ �� �� ����.
	                   // �� ������ ���� �߰����� ������ �޽��� �ڽ��� �Ѱܹޱ� ���� �� �������� Set()�� �ؼ�
					   // ������ ���� �޽��� ť �ε����� �̵� ��Ų ������ �Ʒ����� ������ �߻��ϸ� �ش� ������ ���ؼ��� �޾ƺ� �� �ֵ��� ��.
#endif
	if (FAILED(hr = pSwap->Present(1u, 0u)))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED) // DXGI_ERROR_DEVICE_REMOVED ������ ��쿡 ���� ó������.
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
//		// ���� ü���� ���۵鿡 ���� ��� ������ ������.
//		pTarget.Reset(); // ComPtr�� nullptr �ʱ�ȭ Ȥ�� Reset�� ���� COM ��ü�� release �� �� ����.
//		pDSV.Reset();
//
//		HRESULT hr;
//
//		// ���� �ϴ� ���� ī��Ʈ�� ������ ������.
//		// ������ �ڵ鿡 ���� Ŭ���̾�Ʈ ���� ũ�⿡ �´� ���̿� �ʺ� �ڵ����� ��������.
//		GFX_THROW_INFO(pSwap->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0));
//
//		// ���� ü�� ������ ���۸� ������, ���� Ÿ�� �並 �������.
//		ID3D11Texture2D* pBuffer;
//		GFX_THROW_INFO(pSwap->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBuffer));
//		GFX_THROW_INFO(pDevice->CreateRenderTargetView(pBuffer, NULL, &pTarget));
//		
//		pBuffer->Release(); // COM ��ü�� ���� Release.
//
//		RECT clientRect;
//		GetClientRect(hWnd, &clientRect);
//
//		UINT width = clientRect.right - clientRect.left;
//		UINT height = clientRect.bottom - clientRect.top;
//
//		// ���� ���۵� �������. ���� ���ٽ� state�� ��� ���� ������ ���� ���������ο��� ���� ���� �ʿ� ���� ������,
//		// �ؽ��� �ڿ��� ���� ���ٽ� �丸 ���� �����ؼ� �ٽ� ���������ο� ������.
//		// ���� ���ٽǿ� �ؽ��� ����.
//		Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencil;
//		D3D11_TEXTURE2D_DESC descDepth = {};
//		descDepth.Width = width;                // ���� ������ ������ ũ��(
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
//		// ���� ���ٽ� �ؽ��Ŀ� ���� �� ����.
//		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
//		descDSV.Format = DXGI_FORMAT_D32_FLOAT;
//		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
//		descDSV.Texture2D.MipSlice = 0u;
//		GFX_THROW_INFO(pDevice->CreateDepthStencilView(
//			pDepthStencil.Get(), &descDSV, &pDSV
//		));
//
//		// ���� Ÿ�� ��� ���� ���۸� ��� ���ձ⿡ ������.
//		pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), pDSV.Get());
//
//		// ����Ʈ �缳��.
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
	// �������� ���� ���ڰ� �ִ� ��� ��������.
	if (!info.empty())
	{
		info.pop_back();
	}
}

// what() : ����� ���� ���ڿ��� ������ �����ϴ� �Լ�. std::exception���� ���� �������̵�.
const char* Graphics::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Error String] " << GetErrorString() << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl;
	
	if (!info.empty()) // ����� ���â�� �߰����� ���� ���� ���Խ��Ѽ� �Ѱ��ֵ��� ��.
	{
		oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	}
	
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

// ���� Ÿ�� ���ڿ��� �������ִ� �Լ�.
const char* Graphics::HrException::GetType() const noexcept
{
	return "Custom Graphics Exception";
}

// ���� �ڵ�(HRESULT)�� �������ִ� �Լ�.
HRESULT Graphics::HrException::GetErrorCode() const noexcept
{
	return hr;
}

// dxerr ���̺귯���� ���� ���� ���ڿ��� �������ִ� �Լ�.
std::string Graphics::HrException::GetErrorString() const noexcept
{
	return DXGetErrorStringA(hr);
}

// dxerr ���̺귯���� ���� ������ ���� ���� ���ڿ��� �������ִ� �Լ�.
std::string Graphics::HrException::GetErrorDescription() const noexcept
{
	char buf[512];
	DXGetErrorDescriptionA(hr, buf, sizeof(buf));
	return buf;
}

// ����� ��� â ���� ���� ���ڿ��� �����ϴ� �Լ�.
std::string Graphics::HrException::GetErrorInfo() const noexcept
{
	return info;
}

// ���� Ÿ�� ���ڿ��� �������ִ� �Լ�.
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

// what() : ����� ���� ���ڿ��� ������ �����ϴ� �Լ�. std::exception���� ���� �������̵�.
const char* Graphics::InfoException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

// ���� Ÿ�� ���ڿ��� �������ִ� �Լ�.
const char* Graphics::InfoException::GetType() const noexcept
{
	return "Custom Graphics Info Exception";
}

// ����� ��� â ���� ���� ���ڿ��� �����ϴ� �Լ�.
std::string Graphics::InfoException::GetErrorInfo() const noexcept
{
	return info;
}
#pragma endregion