#pragma once
#include "Window.h"
#include "GameTimer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "Light/PointLight.h"
#include "Mesh.h"
#include <set>

// App : ���ø����̼� Ŭ����. ���α׷��� ��ü ���� ������ ����.
class App
{
public:
	App();
	int Go();
	~App();
private:
	void DoFrame();
	void ShowImguiDemoWindow();
private:
	ImguiManager imgui;
	Window wnd;
	GameTimer gt;
	bool show_demo_window = true;
	float speed_factor = 1.0f;
	Camera cam;
	PointLight light;
	Model nano{ wnd.Gfx(), "Models\\boxy.gltf" };
private:
	float aspectRatio;
	float nearZ;
	float farZ;
};