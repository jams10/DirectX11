#pragma once
#include "../Windows/WindowsHeader.h"

// GDIPlusManager : GDI+ ���̺귯���� �ʱ�ȭ �� ������ ����ϴ� Ŭ����.
class GDIPlusManager
{
public:
	GDIPlusManager();
	~GDIPlusManager();
private:
	static ULONG_PTR token;  // API ȣ���� ���� ��ū. ������ �ڵ�� ����� ����. �ʱ�ȭ�� ���� �� �� �Ѱ���.
	static int refCount;     // ���� ������ ���� ���۷��� ī���� ����.
};