#include "Camera.h"
#include "imgui/imgui.h"
#include "myMath.h"

namespace dx = DirectX;

OvlCamera::OvlCamera() noexcept
{
	Reset();
}

// overall view
DirectX::XMMATRIX OvlCamera::GetMatrix() const noexcept
{
	using namespace dx;

	const dx::XMVECTOR forwardBaseVector = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	// apply the camera rotations to a base vector
	const auto lookVector = XMVector3Transform(forwardBaseVector,
		XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f)
	);
	// camera "top" always faces towards +Y (cannot do a barrel roll)
	const auto camPosition = XMLoadFloat3(&pos);
	const auto camTarget = camPosition + lookVector;
	return XMMatrixLookAtLH(camPosition, camTarget, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
}

void OvlCamera::Update(Window& wnd, float dt) noexcept
{
	if (!wnd.CursorEnabled())
	{
		if (wnd.kbd.KeyIsPressed('W'))
		{
			Translate({ 0.0f,0.0f,dt });
		}
		if (wnd.kbd.KeyIsPressed('A'))
		{
			Translate({ -dt,0.0f,0.0f });
		}
		if (wnd.kbd.KeyIsPressed('S'))
		{
			Translate({ 0.0f,0.0f,-dt });
		}
		if (wnd.kbd.KeyIsPressed('D'))
		{
			Translate({ dt,0.0f,0.0f });
		}
		if (wnd.kbd.KeyIsPressed('R'))
		{
			Translate({ 0.0f,dt,0.0f });
		}
		if (wnd.kbd.KeyIsPressed('F'))
		{
			Translate({ 0.0f,-dt,0.0f });
		}
	}
	while (const auto delta = wnd.mouse.ReadRawDelta())
	{
		if (!wnd.CursorEnabled())
		{
			Rotate((float)delta->x, (float)delta->y);
		}
	}
}

void OvlCamera::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Camera"))
	{
		ImGui::Text("Status");
		ImGui::SliderFloat("MouseSensitivity", &mouseSensitivity, 0.001f, 0.01f, "%f");
		ImGui::SliderFloat("KeyboardSensitivity", &keyboardSensitivity, 15.0f, 200.0f, "%.1f");
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &pos.x, -2000.0f, 2000.0f, "%.1f");
		ImGui::SliderFloat("Y", &pos.y, -2000.0f, 2000.0f, "%.1f");
		ImGui::SliderFloat("Z", &pos.z, -2000.0f, 2000.0f, "%.1f");
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

void OvlCamera::Reset() noexcept
{
	pos = { 0.0f, 180.0f, -650.0f };
	pitch = to_rad(17.0f);
	yaw = 0.0f;
}

void OvlCamera::Rotate(float dx, float dy) noexcept
{
	yaw = wrap_angle(yaw + dx * mouseSensitivity);
	pitch = std::clamp(pitch + dy * mouseSensitivity, 0.995f * -PI / 2.0f, 0.995f * PI / 2.0f);
}

void OvlCamera::Translate(DirectX::XMFLOAT3 translation) noexcept
{
	dx::XMStoreFloat3(&translation, dx::XMVector3Transform(
		dx::XMLoadFloat3(&translation),
		dx::XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f) *
		dx::XMMatrixScaling(keyboardSensitivity, keyboardSensitivity, keyboardSensitivity)
	));
	pos = {
		std::clamp(pos.x + translation.x, -2000.0f, 2000.0f),
		std::clamp(pos.y + translation.y, -2000.0f, 2000.0f),
		std::clamp(pos.z + translation.z, -2000.0f, 2000.0f)
	};
}


// FPP
FPPCamera::FPPCamera(Vehicle& parent)
	:
	parent(parent)
{
	Reset();
}

DirectX::XMMATRIX FPPCamera::GetMatrix() const noexcept
{
	using namespace dx;

	const auto pos = dx::XMVector3Transform(
		dx::XMVectorSet(0.0f, 0.3f, 0.35f, 0.0f),
		parent.GetTransformXM()
	);
	const dx::XMVECTOR forwardBaseVector = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	// apply the camera rotations to a base vector
	const auto lookVector = XMVector3Transform(forwardBaseVector,
		XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f) *
		parent.GetRotationTransformXM()
	);
	const auto camTarget = pos + lookVector;
	const auto Up = XMVector3Transform(dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f),
		parent.GetRotationTransformXM()
	);
	return dx::XMMatrixLookAtLH(
		pos, camTarget, Up);
}

void FPPCamera::Update(Window& wnd, float dt) noexcept
{
	while (const auto delta = wnd.mouse.ReadRawDelta())
	{
		if (!wnd.CursorEnabled())
		{
			Rotate((float)delta->x, (float)delta->y);
		}
	}
}

void FPPCamera::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Camera"))
	{
		ImGui::Text("Status");
		ImGui::SliderFloat("MouseSensitivity", &mouseSensitivity, 0.1f, 3.0f, "%.4f");
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

void FPPCamera::Reset() noexcept
{
	pitch = 0.0f;
	yaw = 0.0f;
}

void FPPCamera::Rotate(float dx, float dy) noexcept
{
	yaw = wrap_angle(yaw + dx * mouseSensitivity);
	pitch = std::clamp(pitch + dy * mouseSensitivity, 0.995f * -PI / 2.0f, 0.995f * PI / 2.0f);
}

// TPP
TPPCamera::TPPCamera(Vehicle& parent)
	:
	parent(parent)
{
	Reset();
}

DirectX::XMMATRIX TPPCamera::GetMatrix() const noexcept
{
	using namespace dx;

	// apply the camera rotations to the pos
	const auto pos = dx::XMVector3Transform(
		dx::XMVectorSet(0.0f, 0.0f, -r, 0.0f),
		XMMatrixRotationRollPitchYaw(theta, phi, 0.0f) *
		parent.GetTransformXM()
	);
	// always look at vehicle
	const auto camTarget = XMVector3Transform(
		XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		parent.GetTransformXM()
	);
	const auto Up = XMVector3Transform(dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f),
		XMMatrixRotationRollPitchYaw(theta, phi, 0.0f) *
		parent.GetRotationTransformXM()
	);
	return dx::XMMatrixLookAtLH(
		pos, camTarget, Up);
}

void TPPCamera::Update(Window& wnd, float dt) noexcept
{
	while (const auto e = wnd.mouse.Read())
	{
		if (!wnd.CursorEnabled())
		{
			if (e->GetType() == Mouse::Event::Type::WheelDown)
			{
				r = std::clamp(r + mouseSensitivity * 180.0, 2.0, 50.0);
			}
			else if (e->GetType() == Mouse::Event::Type::WheelUp)
			{
				r = std::clamp(r - mouseSensitivity * 180.0, 2.0, 50.0);
			}
		}
	}
	while (const auto delta = wnd.mouse.ReadRawDelta())
	{
		if (!wnd.CursorEnabled())
		{
			Rotate((float)delta->x, (float)delta->y);
		}
	}
}

void TPPCamera::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Camera"))
	{
		ImGui::Text("Status");
		ImGui::SliderFloat("MouseSensitivity", &mouseSensitivity, 0.1f, 3.0f, "%.4f");
		ImGui::Text("Position");
		ImGui::SliderFloat("R", &r, 2.0f, 50.0f, "%.1f");
		ImGui::SliderAngle("Theta", &theta, 0.995f * -90.0f, 0.995f * 90.0f);
		ImGui::SliderAngle("Phi", &phi, -180.0f, 180.0f);
		if (ImGui::Button("Reset"))
		{
			Reset();
		}
	}
	ImGui::End();
}

void TPPCamera::Reset() noexcept
{
	r = 10.0f;
	theta = to_rad(60.0f);
	phi = 0.0f;
}

void TPPCamera::Rotate(float dx, float dy) noexcept
{
	phi = wrap_angle(phi + dx * mouseSensitivity);
	theta = std::clamp(theta + dy * mouseSensitivity, 0.995f * -PI / 2.0f, 0.995f * PI / 2.0f);
}
