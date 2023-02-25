#include "Moon.h"
#include "BindableBase.h"
#include "Graphics.h"
#include "Sphere.h"
#include "Surface.h"
#include <array>

Moon::Moon(Graphics& gfx, Earth* earth, float radius, float revoRadius)
	:
	gfx(gfx),
	earth(earth),
	r(revoRadius)
{
	namespace dx = DirectX;

	struct Vertex
	{
		dx::XMFLOAT3 pos;
		dx::XMFLOAT3 n;
		dx::XMFLOAT2 tc;
	};
	auto model = Sphere::MakeTexturedIndependentCapNormals<Vertex>(48, 96);
	model.Transform(dx::XMMatrixScaling(radius, radius, radius));
	AddBind(std::make_unique<VertexBuffer>(gfx, model.vertices));

	AddBind(std::make_unique<Texture>(gfx, Surface::FromFile("Images\\moon_map.jpg"), 0u));
	AddBind(std::make_unique<Sampler>(gfx));

	auto pvs = std::make_unique<VertexShader>(gfx, L"TexturedPhongVS.cso");
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));

	AddBind(std::make_unique<PixelShader>(gfx, L"MoonPhongPS.cso"));

	AddIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.indices));

	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TexCoord",0,DXGI_FORMAT_R32G32_FLOAT,0,24,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};
	AddBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));

	AddBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));


	AddBind(std::make_unique<TransformCbuf>(gfx, *this));
}

void Moon::Update(float dt) noexcept
{
	namespace dx = DirectX;
	roll = wrap_angle(roll + droll * dt);
	pitch = wrap_angle(pitch + dpitch * dt);
	yaw = wrap_angle(yaw + dyaw * dt);
	theta = wrap_angle(theta + dtheta * dt);
	phi = wrap_angle(phi + dphi * dt);
	chi = wrap_angle(chi + dchi * dt);
	dx::XMStoreFloat3(&pos, dx::XMVector3Transform(dx::XMVectorZero(), GetFollowTransformXM()));
	dx::XMStoreFloat3(&transpos, 
		dx::XMVector3Transform(dx::XMVectorZero(), 
			dx::XMMatrixTranslation(0.0f, 0.0f, -r / 2.0f) *
			dx::XMMatrixRotationRollPitchYaw(theta, phi, chi)
		)
	);
}

DirectX::XMMATRIX Moon::GetTransformXM() const noexcept
{
	namespace dx = DirectX;
	return dx::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		dx::XMMatrixTranslation(0.0f, 0.0f, -r) *
		dx::XMMatrixRotationRollPitchYaw(theta, phi, chi) * 
		earth->GetFollowTransformXM();
}

DirectX::XMMATRIX Moon::GetFollowTransformXM() const noexcept
{
	namespace dx = DirectX;
	return dx::XMMatrixTranslation(0.0f, 0.0f, -r) *
		dx::XMMatrixRotationRollPitchYaw(theta, phi, chi) *
		earth->GetFollowTransformXM();
}

DirectX::XMMATRIX Moon::GetRotationTransformXM() const noexcept
{
	namespace dx = DirectX;
	return dx::XMMatrixRotationRollPitchYaw(theta, phi, chi) *
		earth->GetRotationTransformXM();
}

DirectX::XMMATRIX Moon::GetTransTransformXM() const noexcept
{
	namespace dx = DirectX;
	return dx::XMMatrixTranslation(0.0f, 0.0f, -r / 2.0f) *
		dx::XMMatrixRotationRollPitchYaw(theta, phi, chi) *
		earth->GetFollowTransformXM();
}

void Moon::EnableMove() noexcept
{
	droll = 0.0f;
	dpitch = 0.0f;
	dyaw = 0.0f;
	dtheta = 0.0f;
	dphi = -2.0 * PI / (10.0 * 27.3217);
	dchi = 0.0f;
}

void Moon::DisableMove() noexcept
{
	droll = 0.0f;
	dpitch = 0.0f;
	dyaw = 0.0f;
	dtheta = 0.0f;
	dphi = 0.0f;
	dchi = 0.0f;
}
