#include "App.h"

// WinMain : ������ ���� ���α׷��� ������.
int CALLBACK WinMain(
	HINSTANCE hInstance,     // �츮�� �ۼ��� ���α׷� �ν��Ͻ��� ���� �ڵ�.
	HINSTANCE hPrevInstance, // ����� ������� ����. ������ NULL.
	LPSTR	  lpCmdLine,     // �����. C���α׷� ������ char** argv�� ������ ���ø����̼� ����.
	int       nCmdShow)      // �����츦 ������ ���� ����. �ִ�/�ּ�ȭ ���� ���� ����.
{
	try
	{
		return App{}.Go();
	}
	catch (const CustomException& e)
	{
		MessageBoxA(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		MessageBoxA(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBoxA(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;
}