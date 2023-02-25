#pragma once
#include "Earth.h"
#include "Moon.h"
#include "myMath.h"
#include "Window.h"
#include "ConstantBuffers.h"
#include <memory>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Vehicle
{
	friend class Mesh;
public:
	enum class Status
	{
		FOLLOW_EARTH,
		FOLLOW_MOON,
		EARTH_TO_TRANS,
		TRANS_TO_EARTH,
		MOON_TO_TRANS,
		TRANS_TO_MOON,
		FOLLOW_TRANS,
		LANDING_EARTH,
		LANDING_MOON,
		ON_EARTH,
		ON_MOON,
		EARTH_LAUNCH,
		MOON_LAUNCH
	};
public:
	Vehicle(Window& wnd, 
		Earth* earth, Moon* moon, float scale);
	void Update(float dt) noexcept;
	Vehicle::Status GetStatus() noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept;
	DirectX::XMMATRIX GetRotationTransformXM() const noexcept;
	void SetStatus(Vehicle::Status s) noexcept;
	void SetFollow(Drawable& newFollow) noexcept;
	void Draw(Graphics& gfx) noexcept(!IS_DEBUG);
	void ImguiShow() noexcept;
private:
	Status status = Status::FOLLOW_EARTH;
	bool jump = false;
	Window& wnd;
	Graphics& gfx;
	std::vector<std::unique_ptr<Drawable>> meshes;
	// follow
	Earth* earth;
	Moon* moon;
	Drawable* follow;
	float botBound = 0.0f;
	// positional
	float r = 16.0f;
	float roll = -to_rad(90.0f);
	float pitch = 0.0f;
	float yaw = to_rad(90.0f);
	float theta = 0.0f;
	float phi = 0.0f;
	float chi = 0.0f;
	// speed (delta/s)
	float dr = 0.0f;
	float droll = 0.0f;
	float dpitch = 0.0f;
	float dyaw = 0.0f;
	float dtheta = 0.0f;
	float dphi = -2.0f * PI / 10.0f;
	float dchi = 0.0f;
	// oval
	float a = 41.0f;
	float b = 27.0f;
};


class Mesh : public Drawable
{
public:
	Mesh(Vehicle& parent, const aiMesh& mesh, const aiMaterial* const* pMaterials, float scale, float& botBound);
	void Update(float dt) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept;
	DirectX::XMMATRIX GetFollowTransformXM() const noexcept;
	const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept override
	{
		return {};
	}
private:
	Vehicle& parent;
};
