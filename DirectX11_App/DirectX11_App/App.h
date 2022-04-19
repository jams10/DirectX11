#pragma once
#include "Window.h"
#include "GameTimer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "Light/PointLight.h"

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
	ImguiManager imgui;
	Window wnd;
	GameTimer gt;
	std::vector<std::unique_ptr<class Drawable>> drawables; // �׷��� ���� Ŭ�������� Drawable�� ��ӹޱ� ������, Drawable Ÿ������ ������ �� ����.
	bool show_demo_window = true;
	float speed_factor = 1.0f;
	Camera cam;
	PointLight light;
	static constexpr size_t nDrawables = 180;
private:
	float aspectRatio;
	float nearZ;
	float farZ;
};