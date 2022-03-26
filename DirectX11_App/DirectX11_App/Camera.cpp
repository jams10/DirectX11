#include "Camera.h"
#include "imgui/imgui.h"

DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{
	// 카메라 위치
	const auto pos = DirectX::XMVector3Transform(
		DirectX::XMVectorSet(0.0f, 0.0f, -r, 0.0f),
		DirectX::XMMatrixRotationRollPitchYaw(phi, -theta, 0.0f) // phi : 카메라의 pitch, -theta : 카메라의 yaw
																 // 이 회전 값은 중심으로 부터 z축 r만큼 떨어진 다음 회전하게 됨. 카메라 자체가 중심 기준으로 회전.
	);
	// 뷰 변환 행렬
	return DirectX::XMMatrixLookAtLH
		(
			pos,                                         // 카메라 위치 벡터
			DirectX::XMVectorZero(),                     // 카메라가 바라보는 방향 벡터
			DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) // 카메라 위쪽 벡터 
		)
		*   
		DirectX::XMMatrixRotationRollPitchYaw            // 카메라의 위치에 따른 뷰 변환 행렬에 회전 값을 곱해 넘겨 주면,
		(                                                // 이 변환 행렬이 도형들의 정점에 곱해지면서 도형들이 추가적으로 회전하게 됨.
			pitch, -yaw, roll                            // 이 회전 값들은 카메라 쪽에서 설정해 넘겨주기 때문에, 카메라 축 중심으로 도형들이 회전됨.
		);
}

// 카메라 컨트롤 ui 생성 함수.
void Camera::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Camera"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("R", &r, 0.0f, 80.0f, "%.1f");
		ImGui::SliderAngle("Theta", &theta, -180.0f, 180.0f);
		ImGui::SliderAngle("Phi", &phi, -89.0f, 89.0f);
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Roll", &roll, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);
		if (ImGui::Button("Reset"))
		{
			Reset();
		}
	}
	ImGui::End();
}

void Camera::Reset() noexcept
{
	r = 20.0f;
	theta = 0.0f;
	phi = 0.0f;
	pitch = 0.0f;
	yaw = 0.0f;
	roll = 0.0f;
}