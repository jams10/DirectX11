#pragma once
#include "Window.h"
#include "GameTimer.h"

class App
{
public:
	App();
	int Go();
private:
	void DoFrame();
private:
	Window wnd;
	GameTimer gt;
};