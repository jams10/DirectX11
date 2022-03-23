#include "Graphics.h"
#include "dxerr.h"
#include <sstream>
#include <d3dcompiler.h>
#include <cmath>
#include <DirectXMath.h>
#include "GraphicsThrowMacros.h"

#pragma comment(lib,"d3d11.lib")        // Direct3D �Լ����� ���ǵ� ���̺귯���� ��ũ����.
#pragma comment(lib, "D3DCompiler.lib") // ���̴��� ��Ÿ�ӿ� ������ ���� �� ����� �� ������, �츮�� ���̴��� �ҷ����� �Լ��� ����ϱ� ���� ����������. 

Graphics::Graphics(HWND hWnd)
{
	// ���� ü���� ���� ������ ���� ����ü
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = 0;						   // backbuffer �ʺ�
	sd.BufferDesc.Height = 0;						   // backbuffer ����
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

	// depth stencil state ����.
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDSState;
	GFX_THROW_INFO(pDevice->CreateDepthStencilState(&dsDesc, &pDSState));

	// ��� ���ձ⿡ depth state ����.
	pContext->OMSetDepthStencilState(pDSState.Get(), 1u);

	// ���� ���ٽǿ� �ؽ��� ����.
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencil;
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = 800u;                // �ؽ��� ũ��� ���� ü���� ������ ���ۿ� ������.
	descDepth.Height = 600u;
	descDepth.MipLevels = 1u;
	descDepth.ArraySize = 1u;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1u;
	descDepth.SampleDesc.Quality = 0u;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	GFX_THROW_INFO(pDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencil));

	// ���� ���ٽ� �ؽ��Ŀ� ���� �� ����.
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0u;
	GFX_THROW_INFO(pDevice->CreateDepthStencilView(
		pDepthStencil.Get(), &descDSV, &pDSV
	));

	// ��� ���ձ⿡ ���� Ÿ�ٰ� ���� ���ٽ� �� ����.
	pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), pDSV.Get());
}

// ������ ���� ��� �ܰ踦 �ǹ��ϴ� �Լ�. ������ ���� ó���� �� �͵��� ��� ����.(���� ü�� Present)
void Graphics::EndFrame()
{
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

// �� �������� �׸��� ���� ȭ���� ����. ���� ���۸� �ʱ�ȭ���ִ� �Լ�.
void Graphics::ClearBuffer(float red, float green, float blue) noexcept
{
	const float color[] = { red,green,blue,1.0f };
	pContext->ClearRenderTargetView(pTarget.Get(), color); // ���� Ÿ�� �並 �ʱ�ȭ.
	pContext->ClearDepthStencilView(pDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

void Graphics::DrawTestTriangle(float angle, float x, float z)
{
	// ���� ����ü ����.
	struct Vertex
	{
		struct
		{
			float x;
			float y;
			float z;
		} pos;
	};

	// �ﰢ���� ������ ���� �迭 ����. ��ǥ ��ȯ ���� �׳� ��ٷ� NDC ��ǥ�� �����.
	const Vertex vertices[] =
	{
		{ -1.0f,-1.0f,-1.0f },
		{ 1.0f,-1.0f,-1.0f  },
		{ -1.0f,1.0f,-1.0f	},
		{ 1.0f,1.0f,-1.0f	},
		{ -1.0f,-1.0f,1.0f	},
		{ 1.0f,-1.0f,1.0f	},
		{ -1.0f,1.0f,1.0f	},
		{ 1.0f,1.0f,1.0f	},
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	D3D11_BUFFER_DESC bd = {}; // ���� ������.
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = sizeof(vertices);
	bd.StructureByteStride = sizeof(Vertex);

	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = vertices;

	HRESULT hr;
	// ���� ���� ����.
	GFX_THROW_INFO(pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer));
	
	const UINT stride = sizeof(Vertex);
	const UINT offest = 0u;

	// ���� ���� ���������ο� ����.
	pContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offest);

	// �ε��� ���� ����
	const unsigned short indices[] =
	{
		0,2,1, 2,3,1,
		1,3,5, 3,7,5,
		2,6,3, 3,6,7,
		4,5,7, 4,7,6,
		0,4,2, 2,4,6,
		0,1,4, 1,5,4
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

	// �ε��� ���� ���������ο� ����.
	pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);

	// ��ȯ ����� ���� ��� ���� ����.
	struct ConstantBuffer
	{
		DirectX::XMMATRIX transform;
	};
	const ConstantBuffer cb =
	{
		{
			DirectX::XMMatrixTranspose(
				DirectX::XMMatrixRotationZ(angle) *
				DirectX::XMMatrixRotationX(angle) *
				DirectX::XMMatrixTranslation(x, 0.0f, z + 5.0f) *
				DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f/4.0f, 0.5f, 10.f)
			)
		}
	};	

	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
	D3D11_BUFFER_DESC cbd;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;             //  �츮�� matrix�� �� �����Ӹ��� ������Ʈ ���ٰ��̱� ������ DYNAMIC���� ����.
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // CPU���� ���� ����ؼ� ��� ���� ������Ʈ ���� ���̱� ������ CPU_ACCESS_WRITE�� ����.
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = sizeof(cb);
	cbd.StructureByteStride = 0u;                // �츮�� vertex buffer�� index buffer ó�� �迭�� �����Ȱ� �ƴϱ� ������ 0u�� ����.
	D3D11_SUBRESOURCE_DATA csd = {};
	csd.pSysMem = &cb;
	GFX_THROW_INFO(pDevice->CreateBuffer(&cbd, &csd, &pConstantBuffer));

	// ���� ���̴��� ��� ���� ����.
	pContext->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());

	// �ȼ� ���̴����� ������ ���� �� ��� ���� �������.
	struct ConstantBuffer2
	{
		struct
		{
			float r;
			float g;
			float b;
			float a;
		}face_colors[6];
	};

	const ConstantBuffer2 cb2 =
	{
		{
			{1.0f, 0.0f, 1.0f},
			{1.0f, 0.0f, 0.0f},
			{0.0f, 1.0f, 0.0f},
			{0.0f, 0.0f, 1.0f},
			{1.0f, 1.0f, 0.0f},
			{0.0f, 1.0f, 1.0f},
		}
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffe2;
	D3D11_BUFFER_DESC cbd2;
	cbd2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd2.Usage = D3D11_USAGE_DEFAULT;               // ��ȯ ����� ���� ��� ���ۿ� �޸�, �� ���� �� �־��ְ� ���̴��� ������ ���̱� ������ DYNAMIC�� �ƴ�.         
	cbd2.CPUAccessFlags = 0u;                       // DYNAMIC �ƴϹǷ� CPU ���� �÷��� ���� ������.
	cbd2.MiscFlags = 0u;
	cbd2.ByteWidth = sizeof(cb2);
	cbd2.StructureByteStride = 0u;                
	D3D11_SUBRESOURCE_DATA csd2 = {};
	csd2.pSysMem = &cb2;
	GFX_THROW_INFO(pDevice->CreateBuffer(&cbd2, &csd2, &pConstantBuffe2));

	// �ȼ� ���̴��� ��� ���� ������.
	pContext->PSSetConstantBuffers(0u, 1u, pConstantBuffe2.GetAddressOf());

	// �ȼ� ���̴� ����.
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	GFX_THROW_INFO(D3DReadFileToBlob(L"PixelShader.cso", &pBlob));
	GFX_THROW_INFO(pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader));

	// �ȼ� ���̴� ���������ο� ����.
	pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);

	// ���� ���̴� ����.
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	GFX_THROW_INFO(D3DReadFileToBlob(L"VertexShader.cso", &pBlob)); // ���̴��� ����Ʈ �ڵ带 �ҷ���.
	GFX_THROW_INFO(pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader));

	// ���� ���̴� ���������ο� ����.
	pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);

	// �Է� ���̾ƿ� ������.
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	// �Է� ���̾ƿ� ����.
	GFX_THROW_INFO(pDevice->CreateInputLayout(
		ied, (UINT)std::size(ied),
		pBlob->GetBufferPointer(), // ���� ���̴��� ����Ʈ �ڵ带 �Ѱ���.
		pBlob->GetBufferSize(),
		&pInputLayout
	));

	// �Է� ���̾ƿ� ���������ο� ����.
	pContext->IASetInputLayout(pInputLayout.Get());

	// �⺻ ������ triangle list�� ����.
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ����Ʈ ����
	D3D11_VIEWPORT vp;
	vp.Width = 800;
	vp.Height = 600;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0; // ���� Ÿ���� ����Ʈ ��ġ.
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