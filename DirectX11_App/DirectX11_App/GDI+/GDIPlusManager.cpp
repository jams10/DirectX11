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

// GDIPlusManager ������ : 
GDIPlusManager::GDIPlusManager()
{
	if (refCount++ == 0) // ���� ī��Ʈ�� 0�� ��(���� ���̺귯�� �ʱ�ȭ�� �ȵǾ��� ��)�� �ʱ�ȭ�� ���� ����.
	{
		Gdiplus::GdiplusStartupInput input;     // �Է� �ɼ� ���� ����ü. 
		input.GdiplusVersion = 1;               // GDI+�� ����.
		input.DebugEventCallback = nullptr;     // ����� �ݹ� �Լ�
		input.SuppressBackgroundThread = false; // ��׶��� ������ ��� ����.
		Gdiplus::GdiplusStartup(&token, &input, nullptr); // GDI+ ���̺귯�� �ʱ�ȭ
	}
}

GDIPlusManager::~GDIPlusManager()
{
	if (--refCount == 0) // ���� ī��Ʈ�� 0�� �Ǹ� GDI+ ���̺귯�� �ݾ���.
	{
		Gdiplus::GdiplusShutdown(token);
	}
}