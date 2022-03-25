#pragma once
#include "Window.h"
#include "GameTimer.h"
#include "ImguiManager.h"

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
	ImguiManager imgui;
	Window wnd;
	GameTimer gt;
	std::vector<std::unique_ptr<class Drawable>> drawables; // 그려줄 도형 클래스들이 Drawable을 상속받기 때문에, Drawable 타입으로 접근할 수 있음.
	bool show_demo_window = true;
	static constexpr size_t nDrawables = 180;
};