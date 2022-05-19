#pragma once
#include "Window.h"
#include "GameTimer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "Light/PointLight.h"
#include "TestPlane.h"
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
	Model gobber{ wnd.Gfx(),"Models\\gobber\\GoblinX.obj" };
	//Model wall{ wnd.Gfx(),"Models\\brick_wall\\brick_wall.obj" };
	//TestPlane tp{ wnd.Gfx(),1.0 };
	//Model nano{ wnd.Gfx(),"Models\\nano_textured\\nanosuit.obj" };
private:
	float aspectRatio;
	float nearZ;
	float farZ;
};