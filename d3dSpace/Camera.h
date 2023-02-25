#pragma once
#include "Graphics.h"
#include "Window.h"
#include "myMath.h"
#include "Vehicle.h"

class Camera
{
public:
	virtual DirectX::XMMATRIX GetMatrix() const noexcept = 0;
	virtual void Update(Window& wnd, float dt) noexcept = 0;
	virtual void SpawnControlWindow() noexcept = 0;
	virtual void Reset() noexcept = 0;
protected:
	float mouseSensitivity = 0.003f;
};

class OvlCamera : public Camera
{
public:
	OvlCamera() noexcept;
	DirectX::XMMATRIX GetMatrix() const noexcept override;
	void Update(Window& wnd, float dt) noexcept override;
	void SpawnControlWindow() noexcept override;
	void Reset() noexcept override;
	void Rotate(float dx, float dy) noexcept;
	void Translate(DirectX::XMFLOAT3 translation) noexcept;
private:
	DirectX::XMFLOAT3 pos;
	float pitch;
	float yaw;
	float keyboardSensitivity = 50.0f;
};

class FPPCamera : public Camera
{
public:
	FPPCamera(Vehicle& parent);
	DirectX::XMMATRIX GetMatrix() const noexcept override;
	void Update(Window& wnd, float dt) noexcept override;
	void SpawnControlWindow() noexcept override;
	void Reset() noexcept override;
	void Rotate(float dx, float dy) noexcept;
private:
	Vehicle& parent;
	float pitch;
	float yaw;
};

class TPPCamera : public Camera
{
public:
	TPPCamera(Vehicle& parent);
	DirectX::XMMATRIX GetMatrix() const noexcept override;
	void Update(Window& wnd, float dt) noexcept override;
	void SpawnControlWindow() noexcept override;
	void Reset() noexcept override;
	void Rotate(float dx, float dy) noexcept;
private:
	Vehicle& parent;
	float r;
	float theta;
	float phi;
};
