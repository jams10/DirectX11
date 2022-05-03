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
	
	// �� ���� z�� ������ �ٶ󺸴� �⺻ ���Ϳ� ī�޶��� ȸ�� ���� ��������.
	const auto lookVector = DirectX::XMVector3Transform(forwardBaseVector,
		DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f)
	);

	// ī�޶� ��ȯ ����� �������.
	// �ش� ����� ��� ������Ʈ�� ����Ǹ�, ��� ������Ʈ���� ī�޶��� ��ġ�� ȸ�� �������� �������� ��. ��, �� ��ȯ ���.
	// �� ��, ī�޶��� ���� ���ϴ� ���͸� �׻� ���� y�� ������ �ǰ� �Ͽ� ������Ʈ���� barrel roll �Ǵ� ������ ������.
	const auto camPosition = DirectX::XMLoadFloat3(&pos);
	const auto camTarget = DirectX::XMVectorAdd(camPosition, lookVector);
	return DirectX::XMMatrixLookAtLH(camPosition, camTarget, DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
}

// ī�޶� ��Ʈ�� ui ���� �Լ�.
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
	yaw = wrap_angle(yaw + dx * rotationSpeed); // y�� ȸ�� ���� -180 ~ 180 ������ ��������.
	pitch = std::clamp(pitch + dy * rotationSpeed, -PI / 2.0f, PI / 2.0f); // x�� ȸ�� ���� -90 ~ 90 ������ ����.
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