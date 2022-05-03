#pragma once
#include "Window.h"
#include "GameTimer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "Light/PointLight.h"
#include "Mesh.h"
#include <set>

// App : 애플리케이션 클래스. 프로그램의 전체 실행 루프를 가짐.
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
	bool show_demo_window = false;
	float speed_factor = 1.0f;
	Camera cam;
	PointLight light;
	Model nano{ wnd.Gfx(), "Models\\nano.gltf" };
private:
	float aspectRatio;
	float nearZ;
	float farZ;
};