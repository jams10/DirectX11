#include "ImguiManager.h"
#include "imgui.h"

ImguiManager::ImguiManager()
{
	IMGUI_CHECKVERSION();     // ���� üũ
	ImGui::CreateContext();   // Imgui ���̺귯���� ������ �۾��� �ϱ� ���� ���ؽ�Ʈ ����.
	ImGui::StyleColorsDark(); // ui �׸��� ���������� ��������.
	//ImGuiIO& io = ImGui::GetIO();
	//io.Fonts->AddFontFromFileTTF("Fonts\\GimpoGothic\\Bold.ttf", 16.0f, NULL, io.Fonts->GetGlyphRangesKorean());
}

ImguiManager::~ImguiManager()
{
	ImGui::DestroyContext();  // ������ ���ؽ�Ʈ ����.
}