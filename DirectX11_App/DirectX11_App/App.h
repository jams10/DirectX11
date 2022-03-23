#pragma once
#include "Window.h"
#include "GameTimer.h"

// App : 애플리케이션 클래스. 프로그램의 전체 실행 루프를 가짐.
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