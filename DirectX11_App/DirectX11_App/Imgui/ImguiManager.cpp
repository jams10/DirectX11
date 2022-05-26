#include "ImguiManager.h"
#include "imgui.h"

ImguiManager::ImguiManager()
{
	IMGUI_CHECKVERSION();     // 버전 체크
	ImGui::CreateContext();   // Imgui 라이브러리를 가지고 작업을 하기 위한 컨텍스트 생성.
	ImGui::StyleColorsDark(); // ui 테마를 검정색으로 설정해줌.
	//ImGuiIO& io = ImGui::GetIO();
	//io.Fonts->AddFontFromFileTTF("Fonts\\GimpoGothic\\Bold.ttf", 16.0f, NULL, io.Fonts->GetGlyphRangesKorean());
}

ImguiManager::~ImguiManager()
{
	ImGui::DestroyContext();  // 생성한 컨텍스트 제거.
}