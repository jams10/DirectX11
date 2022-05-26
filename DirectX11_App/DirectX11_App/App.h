#pragma once
#include "Window.h"
#include "GameTimer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "Light/PointLight.h"
#include "TestPlane.h"
#include "Mesh.h"
#include <set>

// App : ���ø����̼� Ŭ����. ���α׷��� ��ü ���� ������ ����.
class App
{
public:
	App(const std::string& commandLine = "");
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
	/*Model gobber{ wnd.Gfx(),"Models\\gobber\\GoblinX.obj",6.0f };
	Model wall{ wnd.Gfx(),"Models\\brick_wall\\brick_wall.obj",6.0f };
	TestPlane tp{ wnd.Gfx(),6.0 };
	Model nano{ wnd.Gfx(),"Models\\nano_textured\\nanosuit.obj",2.0f };*/
	Model sponza{ wnd.Gfx(), "Models\\sponza\\sponza.obj", 1.0f / 20.f };
private:
	std::string commandLine;
	float aspectRatio;
	float nearZ;
	float farZ;
};