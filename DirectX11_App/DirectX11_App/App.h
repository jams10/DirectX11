#pragma once
#include "Window.h"
#include "GameTimer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "Light/PointLight.h"
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
	void SpawnSimulationWindow() noexcept;
	void SpawnBoxWindowManagerWindow() noexcept;
	void SpawnBoxWindows() noexcept;
private:
	ImguiManager imgui;
	Window wnd;
	GameTimer gt;
	std::vector<std::unique_ptr<class Drawable>> drawables; // �׷��� ���� Ŭ�������� Drawable�� ��ӹޱ� ������, Drawable Ÿ������ ������ �� ����.
	std::vector<class Box*> boxes;
	bool show_demo_window = true;
	float speed_factor = 1.0f;
	Camera cam;
	PointLight light;
	static constexpr size_t nDrawables = 180;
	std::optional<int> comboBoxIndex;
	std::set<int> boxControlIds;
private:
	float aspectRatio;
	float nearZ;
	float farZ;
};