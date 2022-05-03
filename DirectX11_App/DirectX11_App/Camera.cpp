#include "Camera.h"
#include "imgui/imgui.h"
#include "CustomMath.h"

Camera::Camera() noexcept
{
	Reset();
}

DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{
	return DirectX::XMMatrixTranslation(-pos.x, -pos.y, -pos.z) *
		   DirectX::XMMatrixRotationRollPitchYaw(-pitch, -yaw, 0.0f);	
}

// 카메라 컨트롤 ui 생성 함수.
void Camera::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Camera"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &pos.x, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Y", &pos.y, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Z", &pos.z, -80.0f, 80.0f, "%.1f");
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Pitch", &pitch, -90.0f, 90.0f);
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
	pos = { 0.0f,7.5f,-18.0f };
	pitch = 0.0f;
	yaw = 0.0f;
}

void Camera::Rotate(float dx, float dy) noexcept
{
	yaw = wrap_angle(yaw + dx * rotationSpeed); // y축 회전 값을 -180 ~ 180 값으로 제한해줌.
	pitch = std::clamp(pitch + dy * rotationSpeed, -PI / 2.0f, PI / 2.0f); // x축 회전 값을 -90 ~ 90 값으로 제한.
}

void Camera::Translate(DirectX::XMFLOAT3 translation) noexcept
{
	DirectX::XMStoreFloat3(&translation, DirectX::XMVector3Transform(
		DirectX::XMLoadFloat3(&translation),
		DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f) *
		DirectX::XMMatrixScaling(travelSpeed, travelSpeed, travelSpeed)
	));
	pos = {
		pos.x + translation.x,
		pos.y + translation.y,
		pos.z + translation.z
	};
}