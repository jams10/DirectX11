#include "DxgiInfoManager.h"
#include "Window.h"
#include "Graphics.h"
#include <memory>

#pragma comment(lib, "dxguid.lib")

#define GFX_THROW_NOINFO(hrcall) if( FAILED( hr = (hrcall) ) ) throw Graphics::HrException( __LINE__,__FILE__,hr )

DxgiInfoManager::DxgiInfoManager()
{
	// DXGIGetDebugInterface�� �Լ� �ñ״��ĸ� ����
	// DXGIGetDebugInterface�� ���� Ÿ���� HRESULT, ���ڰ� REFIID, void**, ȣ�� �Ծ��� WINAPI(__stdcall)�� �Լ��� ���� �Լ� �����ͷ� ��.
	typedef HRESULT(WINAPI* DXGIGetDebugInterface)(REFIID, void**);

	// DXGIGetDebugInterface �Լ��� �ִ� dll�� �ε���.
	const auto hModDxgiDebug = LoadLibraryExA("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (hModDxgiDebug == nullptr)
	{
		throw WND_LAST_EXCEPT();
	}

	// dll�� �ִ� DXGIGetDebugInterface�� �ּҸ� ������.
	const auto DxgiGetDebugInterface = reinterpret_cast<DXGIGetDebugInterface>(
		reinterpret_cast<void*>(GetProcAddress(hModDxgiDebug, "DXGIGetDebugInterface"))
		);
	if (DxgiGetDebugInterface == nullptr)
	{
		throw WND_LAST_EXCEPT();
	}

	HRESULT hr;
	// �츮�� ������ dll�� �ִ� DXGIGetDebugInterface �Լ��� ���� IDXGIInfoQueue�� �޾ƿ� ComPtr�� �־���.
	GFX_THROW_NOINFO(DxgiGetDebugInterface(__uuidof(IDXGIInfoQueue), &pDxgiInfoQueue));
}

// ����� �޽��� ���� ���� �Լ�.
void DxgiInfoManager::Set() noexcept
{
	/*
	<GetNumStoredMessages�� �Ű������� DXGI_DEBUG_ID>
		����� �޽����� ������ ��ü�� �ĺ��ϴ� GUID(Globally Unique Identifier)
		DXGI_DEBUG_ALL�� ��� Direct3D, DXGI ��ü �� ���ø����̼ǿ� ���� ����� �޽����� �޾��� ���� �ǹ�.
	*/
	/*
		�Ʒ� GetMessages�� �Լ��� ȣ�� �� �� �ε����� �ϳ��� ���鼭 InfoQueue�� ����� ����� �޽������� �ϳ� �� ������ ���ε�,
		������ �������� ������ ��� ����� �޽����� ��� �ڵ�� ������ ������ �˱� �����.
		�׷��� Set �Լ����� next��� ������ ������ ����ϴ� �ε����� ���� Set ȣ�� ���� ������ ����� �޽������� ������ �����Ͽ�,
		���� �ֱ� �Լ� ȣ�⿡ ���ؼ��� ���� ���� ����� �޽������� ���� �� �ֵ��� �ߴ�.
		��, ������ Set() ȣ�� ���Ŀ� �Ͼ ������ ���ؼ��� GetMessages�� ���� ���� �޽����� ���� �� �ִ� ���̴�.
	*/

	next = pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
}

// Debug Information Queue�� ���� ���� �޽������� ������ ���ڿ� ���͸� �����ϴ� �Լ�.
std::vector<std::string> DxgiInfoManager::GetMessages() const
{
	std::vector<std::string> messages;
	const auto end = pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
	for (auto i = next; i < end; i++)
	{
		HRESULT hr;
		SIZE_T messageLength;
		
		// �ε��� i �޽����� ũ�⸦ byte�� �޾ƿ�.
		GFX_THROW_NOINFO(pDxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &messageLength));
		
		// �޽����� ���� �޸𸮸� �Ҵ���.
		auto bytes = std::make_unique<byte[]>(messageLength);
		auto pMessage = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get());
		
		// �޽����� �޾ƿͼ� �� ������ ���Ϳ� ����.
		GFX_THROW_NOINFO(pDxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, pMessage, &messageLength));
		messages.emplace_back(pMessage->pDescription);
	}
	return messages;
}