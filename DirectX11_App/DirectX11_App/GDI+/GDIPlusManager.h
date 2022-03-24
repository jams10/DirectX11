#pragma once
#include "../Windows/WindowsHeader.h"

// GDIPlusManager : GDI+ 라이브러리의 초기화 및 해제를 담당하는 클래스.
class GDIPlusManager
{
public:
	GDIPlusManager();
	~GDIPlusManager();
private:
	static ULONG_PTR token;  // API 호출을 위한 토큰. 윈도우 핸들과 비슷한 개념. 초기화와 해제 할 때 넘겨줌.
	static int refCount;     // 참조 추적을 위한 레퍼런스 카운팅 변수.
};