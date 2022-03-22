#pragma once
#include "WindowsHeader.h"
#include <vector>
#include <string>

class DxgiInfoManager
{
public:
	DxgiInfoManager();
	~DxgiInfoManager();
	DxgiInfoManager(const DxgiInfoManager&) = delete;
	DxgiInfoManager& operator=(const DxgiInfoManager&) = delete;
	void Set() noexcept;
	std::vector<std::string> GetMessages() const;
private:
	unsigned long long next = 0u;
	struct IDXGIInfoQueue* pDxgiInfoQueue = nullptr; // Debug Information Queue를 제어하는 인터페이스. Debug Layer가 켜졌을 때만 사용할 수 있음.
													 // 이 인터페이스를 통해 Debug Output의 결과를 MessageBox에 뿌려줄 수 있도록 할 것임.
};