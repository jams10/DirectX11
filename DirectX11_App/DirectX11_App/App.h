#pragma once
#include "Window.h"
#include "GameTimer.h"

// App : ���ø����̼� Ŭ����. ���α׷��� ��ü ���� ������ ����.
class App
{
public:
	App();
	int Go();
	~App();
private:
	void DoFrame();
private:
	Window wnd;
	GameTimer gt;
	std::vector<std::unique_ptr<class Box>> boxes;
};