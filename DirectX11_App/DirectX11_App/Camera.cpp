#include "Camera.h"
#include "imgui/imgui.h"
#include "CustomMath.h"

Camera::Camera() noexcept
{
	Reset();
}

DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{
	const DirectX::XMVECTOR forwardBaseVector = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	
	// 앞 양의 z축 방향을 바라보는 기본 벡터에 카메라의 회전 값을 적용해줌.
	const auto lookVector = DirectX::XMVector3Transform(forwardBaseVector,
		DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f)
	);

	// 카메라 변환 행렬을 만들어줌.
	// 해당 행렬은 모든 오브젝트에 적용되며, 모든 오브젝트들이 카메라의 위치와 회전 기준으로 놓여지게 함. 즉, 뷰 변환 행렬.
	// 이 때, 카메라의 위를 향하는 벡터를 항상 양의 y축 방향이 되게 하여 오브젝트들이 barrel roll 되는 현상을 막아줌.
	const auto camPosition = DirectX::XMLoadFloat3(&pos);
	const auto camTarget = DirectX::XMVectorAdd(camPosition, lookVector);
	return DirectX::XMMatrixLookAtLH(camPosition, camTarget, DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
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
		ImGui::SliderAngle("Pitch", &pitch, 0.995f * -90.0f, 0.995f * 90.0f);
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
	pos = { -13.5f,6.0f,3.5f };
	pitch = 0.0f;
	yaw = PI / 2.0f;
}

void Camera::Rotate(float dx, float dy) noexcept
{
	yaw = wrap_angle(yaw + dx * rotationSpeed); // y축 회전 값을 -180 ~ 180 값으로 제한해줌.
	pitch = std::clamp(pitch + dy * rotationSpeed, 0.995f * -PI / 2.0f, 0.995f * PI / 2.0f);
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

DirectX::XMFLOAT3 Camera::GetPos() const noexcept
{
	return pos;
}