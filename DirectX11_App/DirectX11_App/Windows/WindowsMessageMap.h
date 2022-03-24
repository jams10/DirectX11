#pragma once
#include <unordered_map>
#include <Windows.h>

// WindowsMessageMap : ������ �޽������� {�޽��� ��(16����) Ű, ���ڿ� ���ͷ� ��}�� ������ ������ ������ �޽����� �м��� �� �ֵ��� �����ִ� Ŭ����.
class WindowsMessageMap
{
public:
	WindowsMessageMap();
	std::string operator()(DWORD msg, WPARAM wp, LPARAM lp) const; // () �����ڸ� �����ε�. Fuctor. Ŭ���� ��ü�� �Լ� ó�� ����� �� �ְ� ��.
private:
	std::unordered_map<DWORD, std::string> map;
};