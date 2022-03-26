#include "Camera.h"
#include "imgui/imgui.h"

DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{
	// ī�޶� ��ġ
	const auto pos = DirectX::XMVector3Transform(
		DirectX::XMVectorSet(0.0f, 0.0f, -r, 0.0f),
		DirectX::XMMatrixRotationRollPitchYaw(phi, -theta, 0.0f) // phi : ī�޶��� pitch, -theta : ī�޶��� yaw
																 // �� ȸ�� ���� �߽����� ���� z�� r��ŭ ������ ���� ȸ���ϰ� ��. ī�޶� ��ü�� �߽� �������� ȸ��.
	);
	// �� ��ȯ ���
	return DirectX::XMMatrixLookAtLH
		(
			pos,                                         // ī�޶� ��ġ ����
			DirectX::XMVectorZero(),                     // ī�޶� �ٶ󺸴� ���� ����
			DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) // ī�޶� ���� ���� 
		)
		*   
		DirectX::XMMatrixRotationRollPitchYaw            // ī�޶��� ��ġ�� ���� �� ��ȯ ��Ŀ� ȸ�� ���� ���� �Ѱ� �ָ�,
		(                                                // �� ��ȯ ����� �������� ������ �������鼭 �������� �߰������� ȸ���ϰ� ��.
			pitch, -yaw, roll                            // �� ȸ�� ������ ī�޶� �ʿ��� ������ �Ѱ��ֱ� ������, ī�޶� �� �߽����� �������� ȸ����.
		);
}

// ī�޶� ��Ʈ�� ui ���� �Լ�.
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