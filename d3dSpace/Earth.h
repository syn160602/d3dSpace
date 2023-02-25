#pragma once
#include "Drawable.h"
#include "myMath.h"
#include "ConstantBuffers.h"

class Earth : public Drawable
{
	friend class Moon;
	friend class Vehicle;
public:
	Earth(Graphics& gfx, float radius = 10.0, float revoRadius = 300.0);
	void Update(float dt) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept;
	DirectX::XMMATRIX GetFollowTransformXM() const noexcept override;
	DirectX::XMMATRIX GetRotationTransformXM() const noexcept;
	const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept override
	{
		return {};
	}
	void EnableMove() noexcept;
	void DisableMove() noexcept;
public:
	DirectX::XMFLOAT3 pos;
private:
	Graphics& gfx;
	// positional
	float r;
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
	float theta = 0.0f;
	float phi = 0.0f;
	float chi = 0.0f;
	// speed (delta/s)
	float droll = 0.0f;
	float dpitch = 0.0f;
	float dyaw = 0.0f;	//move speed:  -2.0 * PI / 10.0;
	float dtheta = 0.0f;
	float dphi = 0.0f;	// move speed: -2.0 * PI / (10.0 * 365.2422);
	float dchi = 0.0f;
};