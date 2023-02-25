#pragma once
#include "Drawable.h"
#include "myMath.h"
#include "ConstantBuffers.h"
#include "Earth.h"

class Moon : public Drawable
{
	friend class Vehicle;
public:
	Moon(Graphics& gfx, Earth* earth, float radius = 2.4, float revoRadius = 50.0);
	void Update(float dt) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept;
	DirectX::XMMATRIX GetFollowTransformXM() const noexcept override;
	DirectX::XMMATRIX GetRotationTransformXM() const noexcept;
	DirectX::XMMATRIX GetTransTransformXM() const noexcept;
	const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept override
	{
		return {};
	}
	void EnableMove() noexcept;
	void DisableMove() noexcept;
public:
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 transpos;
private:
	Graphics& gfx;
	Earth* earth;
	// positional
	float r;
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
	float theta = 0.0f;
	float phi = 0.0f;
	float chi = 0.0f;// to_rad(5.1454f);		// moon path
	// speed (delta/s)
	float droll = 0.0f;
	float dpitch = 0.0f;
	float dyaw = 0.0f;
	float dtheta = 0.0f;
	float dphi = 0.0f;	// move speed: -2.0 * PI / (10.0 * 27.3217);
	float dchi = 0.0f;
};