#define FULL_WIN_API
#include "../Windows/WindowsHeader.h"
#include "GDIPlusManager.h"
#include <algorithm>

namespace Gdiplus
{
	using std::min;
	using std::max;
}
#include <gdiplus.h>

ULONG_PTR GDIPlusManager::token = 0;
int GDIPlusManager::refCount = 0;

// GDIPlusManager 생성자 : 
GDIPlusManager::GDIPlusManager()
{
	if (refCount++ == 0) // 참조 카운트가 0일 때(아직 라이브러리 초기화가 안되었을 때)만 초기화를 진행 해줌.
	{
		Gdiplus::GdiplusStartupInput input;     // 입력 옵션 정의 구조체. 
		input.GdiplusVersion = 1;               // GDI+의 버전.
		input.DebugEventCallback = nullptr;     // 디버그 콜백 함수
		input.SuppressBackgroundThread = false; // 백그라운드 스레드 허용 여부.
		Gdiplus::GdiplusStartup(&token, &input, nullptr); // GDI+ 라이브러리 초기화
	}
}

GDIPlusManager::~GDIPlusManager()
{
	if (--refCount == 0) // 참조 카운트가 0이 되면 GDI+ 라이브러리 닫아줌.
	{
		Gdiplus::GdiplusShutdown(token);
	}
}