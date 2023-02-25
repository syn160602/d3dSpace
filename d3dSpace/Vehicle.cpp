#include "Vehicle.h"
#include "BindableBase.h"
#include "Graphics.h"
#include "GraphicThrowMacros.h"
#include "imgui/imgui.h"
#include "Surface.h"

Vehicle::Vehicle(Window& wnd, 
	Earth* earth, Moon* moon, float scale)
	:
	wnd(wnd),
	gfx(wnd.Gfx()),
	earth(earth),
	moon(moon),
	follow(earth)
{
	namespace dx = DirectX;

	Assimp::Importer imp;
	const auto pModel = imp.ReadFile("models\\export3dcoat.obj",
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals
	);

	for (size_t i = 0; i < pModel->mNumMeshes; ++i)
	{
		meshes.emplace_back(std::make_unique<Mesh>(*this, *pModel->mMeshes[i], pModel->mMaterials, scale, botBound));
	}
}

void Vehicle::Update(float dt) noexcept
{
	namespace dx = DirectX;

	bool update = true;
	while (update) {
		update = false;
		switch (status) {
		case Status::EARTH_TO_TRANS:
		{
			// press q to cancel
			if (wnd.kbd.KeyIsPressed('Q')) {
				status = Status::FOLLOW_EARTH;
				update = true;
				break;
			}
			// whether reach the change point
			float targetAng = wrap_angle(
				dx::XMVectorGetX(dx::XMVector3AngleBetweenVectors(
					dx::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f),
					dx::XMLoadFloat3(&moon->transpos))
				) - PI
			);
			targetAng = moon->phi > 0 ? targetAng : -targetAng;

			if (wrap_angle(phi - targetAng) <= (float)(-1e-5) && jump) {
				status = Status::FOLLOW_TRANS;
				phi = wrap_angle(PI);
				jump = false;
				update = true;
				break;
			}
			else if (wrap_angle(phi - targetAng) > 0.0f)
				jump = true;
			// trans condition not satisfied, go on follow
		}
		case Status::FOLLOW_EARTH:
		{
			r = 16.0f;
			roll = wrap_angle(roll + droll * dt);
			pitch = wrap_angle(pitch + dpitch * dt);
			yaw = wrap_angle(yaw + dyaw * dt);
			phi = wrap_angle(phi + dphi * dt);
			// disable change when waiting
			if (status == Status::FOLLOW_EARTH)
			{
				if (wnd.kbd.KeyIsPressed('J'))
					status = Status::EARTH_TO_TRANS;
				else if (wnd.kbd.KeyIsPressed('L'))
					status = Status::LANDING_EARTH;
			}
			break;
		}

		case Status::MOON_TO_TRANS:
		{
			// press q to cancel
			if (wnd.kbd.KeyIsPressed('Q')) {
				status = Status::FOLLOW_MOON;
				update = true;
				break;
			}

			if (wrap_angle(phi) <= (float)(-1e-5) && jump) {
				status = Status::FOLLOW_TRANS;
				phi = wrap_angle(0.0f);
				jump = false;
				update = true;
				break;
			}
			else if (wrap_angle(phi) > 0.0f)
				jump = true;
			// trans condition not satisfied, go on follow
		}
		case Status::FOLLOW_MOON:
		{
			r = 16.0f;
			roll = wrap_angle(roll + droll * dt);
			pitch = wrap_angle(pitch + dpitch * dt);
			yaw = wrap_angle(yaw + dyaw * dt);
			phi = wrap_angle(phi + dphi * dt);
			// disable change when waiting
			if (status == Status::FOLLOW_MOON) {
				if (wnd.kbd.KeyIsPressed('J'))
					status = Status::MOON_TO_TRANS;
				else if (wnd.kbd.KeyIsPressed('L'))
					status = Status::LANDING_MOON;
			}
			break;
		}

		case Status::TRANS_TO_EARTH:
		{
			// press q to cancel
			if (wnd.kbd.KeyIsPressed('Q')) {
				status = Status::FOLLOW_TRANS;
				update = true;
				break;
			}

			if (phi > (float)(-1e-4) && jump)
			{
				status = Status::FOLLOW_EARTH;
				dphi = -2.0 * PI / 10.0;
				float localAng = wrap_angle(
					dx::XMVectorGetX(dx::XMVector3AngleBetweenVectors(
						dx::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f),
						dx::XMLoadFloat3(&moon->transpos))
					) - PI
				);
				phi = moon->phi > 0 ? localAng : -localAng;
				SetFollow(*earth);

				jump = false;
				update = true;
				break;
			}
			else if (phi < 0.0f)
				jump = true;
			// follow trans
			roll = wrap_angle(roll + droll * dt);
			pitch = wrap_angle(pitch + dpitch * dt);
			yaw = wrap_angle(yaw + dyaw * dt);

			if (phi > PI / 16.0f && phi < PI / 2.0f || phi > -PI * 15.0f / 16.0f && phi < -PI / 2.0f)
				dphi = std::max(dphi - 0.0005f * PI / 10.0f, -2.1f * PI / 10.0f);
			else if (phi > PI / 2.0f && phi < PI || phi > -PI / 2.0f && phi < 0.0f)
				dphi = std::min(dphi + 0.001f * PI / 10.0f, -1.5f * PI / 10.0f);
			phi = wrap_angle(phi + dphi * dt);

			r = sqrtf(pow(a * cosf(phi), 2.0f) + pow(b * sinf(phi), 2.0f));
			break;
		}
		case Status::TRANS_TO_MOON:
		{
			// press q to cancel
			if (wnd.kbd.KeyIsPressed('Q')) {
				status = Status::FOLLOW_TRANS;
				update = true;
				break;
			}

			if (phi < (float)(-1e-4) && jump)
			{
				status = Status::FOLLOW_MOON;
				dphi = -2.0 * PI / 10.0;
				phi = 0.0f;
				SetFollow(*moon);

				jump = false;
				update = true;
				break;
			}
			else if (phi > 0.0f)
				jump = true;
			// follow trans
			roll = wrap_angle(roll + droll * dt);
			pitch = wrap_angle(pitch + dpitch * dt);
			yaw = wrap_angle(yaw + dyaw * dt);

			if (phi > PI / 16.0f && phi < PI / 2.0f || phi > -PI * 15.0f / 16.0f && phi < -PI / 2.0f)
				dphi = std::max(dphi - 0.0005f * PI / 10.0f, -2.1f * PI / 10.0f);
			else if (phi > PI / 2.0f && phi < PI || phi > -PI / 2.0f && phi < 0.0f)
				dphi = std::min(dphi + 0.001f * PI / 10.0f, -1.5f * PI / 10.0f);
			phi = wrap_angle(phi + dphi * dt);

			r = sqrtf(pow(a * cosf(phi), 2.0f) + pow(b * sinf(phi), 2.0f));
			break;
		}
		case Status::FOLLOW_TRANS:
		{
			roll = wrap_angle(roll + droll * dt);
			pitch = wrap_angle(pitch + dpitch * dt);
			yaw = wrap_angle(yaw + dyaw * dt);

			if (phi > PI / 16.0f && phi < PI / 2.0f || phi > -PI * 15.0f / 16.0f && phi < -PI / 2.0f)
				dphi = std::max(dphi - 0.0005f * PI / 10.0f, -2.1f * PI / 10.0f);
			else if (phi > PI / 2.0f && phi < PI || phi > -PI / 2.0f && phi < 0.0f)
				dphi = std::min(dphi + 0.001f * PI / 10.0f, -1.5f * PI / 10.0f);
			phi = wrap_angle(phi + dphi * dt);

			r = sqrtf(pow(a * cosf(phi), 2.0f) + pow(b * sinf(phi), 2.0f));

			if (wnd.kbd.KeyIsPressed('J'))
			{
				if (phi >= 0.0f)
					status = Status::TRANS_TO_MOON;
				else
					status = Status::TRANS_TO_EARTH;
			}
			break;
		}

		case Status::LANDING_EARTH:
		{
			roll = wrap_angle(roll + droll * dt);
			pitch = wrap_angle(pitch + dpitch * dt);
			yaw = wrap_angle(yaw + dyaw * dt);

			float targetR = 10.0f - botBound;	// TODO: hard coding
			if (dphi > (earth->dyaw))
			{
				// accel
				dphi = std::max(dphi - PI / 10.0f * dt, 
					earth->dyaw
				);
			}
			else
			{
				// decel
				dphi = std::min(dphi + PI / 10.0f * dt,
					earth->dyaw
				);
			}
			dr = std::max(dr + (r > -botBound / 2.0f + 13.0f ? 0.01f : -0.01f) * dt, (float)1e-3);
			phi = wrap_angle(phi + dphi * dt);
			r = std::max(r - dr, targetR);
			if (r <= targetR)
			{
				status = Status::ON_EARTH;
				dphi = earth->dyaw;
			}
			break;
		}
		case Status::LANDING_MOON:
		{
			roll = wrap_angle(roll + droll * dt);
			pitch = wrap_angle(pitch + dpitch * dt);
			yaw = wrap_angle(yaw + dyaw * dt);

			float targetR = 2.4f - botBound;	// TODO: hard coding
			if (dphi > (moon->dyaw))
			{
				// accel
				dphi = std::max(dphi - PI / 25.0f * dt,
					moon->dyaw
				);
			}
			else
			{
				// decel
				dphi = std::min(dphi + PI / 25.0f * dt,
					moon->dyaw
				);
			}
			dr = std::max(dr + (r > -botBound / 2.0f + 9.2f ? 0.01f : -0.01f) * dt, (float)1e-3);
			phi = wrap_angle(phi + dphi * dt);
			r = std::max(r - dr, targetR);
			if (r <= targetR)
			{
				status = Status::ON_MOON;
				dphi = moon->dyaw;
			}
			break;
		}
		case Status::ON_EARTH:
		{
			dphi = earth->dyaw;
			r = 10.0f - botBound;
			roll = wrap_angle(roll + droll * dt);
			pitch = wrap_angle(pitch + dpitch * dt);
			yaw = wrap_angle(yaw + dyaw * dt);
			phi = wrap_angle(phi + dphi * dt);

			if (wnd.kbd.KeyIsPressed('L'))
			{
				status = Status::EARTH_LAUNCH;
			}
			break;
		}
		case Status::ON_MOON:
		{
			dphi = moon->dyaw;
			r = 2.4f - botBound;
			roll = wrap_angle(roll + droll * dt);
			pitch = wrap_angle(pitch + dpitch * dt);
			yaw = wrap_angle(yaw + dyaw * dt);
			phi = wrap_angle(phi + dphi * dt);

			if (wnd.kbd.KeyIsPressed('L'))
			{
				status = Status::MOON_LAUNCH;
			}
			break;
		}
		case Status::EARTH_LAUNCH:
		{
			roll = wrap_angle(roll + droll * dt);
			pitch = wrap_angle(pitch + dpitch * dt);
			yaw = wrap_angle(yaw + dyaw * dt);

			if (dphi > -2.0f * PI / 10.0f)
			{
				// accel
				dphi = std::max(dphi - PI / 10.0f * dt,
					-2.0f * PI / 20.0f
				);
			}
			else
			{
				// decel
				dphi = std::min(dphi + PI / 10.0f * dt,
					-2.0f * PI / 20.0f
				);
			}
			dr = std::max(dr + (r < -botBound / 2.0f + 13.0f ? 0.01f : -0.01f) * dt, (float)1e-3);
			phi = wrap_angle(phi + dphi * dt);
			r = std::min(r + dr, 16.0f);
			if (r >= 16.0f)
			{
				status = Status::FOLLOW_EARTH;
				dphi = -2.0f * PI / 10.0f;
			}
			break;
		}
		case Status::MOON_LAUNCH:
		{
			roll = wrap_angle(roll + droll * dt);
			pitch = wrap_angle(pitch + dpitch * dt);
			yaw = wrap_angle(yaw + dyaw * dt);

			if (dphi > -2.0f * PI / 10.0f)
			{
				// accel
				dphi = std::max(dphi - PI / 25.0f * dt,
					-2.0f * PI / 10.0f
				);
			}
			else
			{
				// decel
				dphi = std::min(dphi + PI / 25.0f * dt,
					-2.0f * PI / 10.0f
				);
			}
			dr = std::max(dr + (r < -botBound / 2.0f + 9.2f ? 0.01f : -0.01f) * dt, (float)1e-3);
			phi = wrap_angle(phi + dphi * dt);
			r = std::min(r + dr, 16.0f);
			if (r >= 16.0f)
			{
				status = Status::FOLLOW_MOON;
				dphi = -2.0f * PI / 10.0f;
			}
			break;
		}
		}
	}

	for (size_t i = 0; i < meshes.size(); ++i)
	{
		meshes[i]->Update(dt);
	}
}

Vehicle::Status Vehicle::GetStatus() noexcept
{
	return status;
}

DirectX::XMMATRIX Vehicle::GetTransformXM() const noexcept
{
	namespace dx = DirectX;
	switch (status)
	{
	case Status::FOLLOW_TRANS:
	case Status::TRANS_TO_EARTH:
	case Status::TRANS_TO_MOON:
		return dx::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
			dx::XMMatrixTranslation(0.0f, 0.0f, -r) *
			dx::XMMatrixRotationRollPitchYaw(theta, phi, chi) *
			moon->GetTransTransformXM();
	default:
		return dx::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
			dx::XMMatrixTranslation(0.0f, 0.0f, -r) *
			dx::XMMatrixRotationRollPitchYaw(theta, phi, chi) *
			follow->GetFollowTransformXM();
	}
}

DirectX::XMMATRIX Vehicle::GetRotationTransformXM() const noexcept
{
	namespace dx = DirectX;
	switch (status)
	{
	case Status::FOLLOW_EARTH:
	case Status::EARTH_TO_TRANS:
	case Status::LANDING_EARTH:
	case Status::ON_EARTH:
	case Status::EARTH_LAUNCH:
		return dx::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
			dx::XMMatrixRotationRollPitchYaw(theta, phi, chi) *
			earth->GetRotationTransformXM();
	default:
		return dx::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
			dx::XMMatrixRotationRollPitchYaw(theta, phi, chi) *
			moon->GetRotationTransformXM();
	}
}

void Vehicle::SetStatus(Vehicle::Status s) noexcept
{
	status = s;
}

void Vehicle::SetFollow(Drawable& newFollow) noexcept
{
	follow = &newFollow;
}

void Vehicle::Draw(Graphics& gfx) noexcept(!IS_DEBUG)
{
	for (auto& mesh : meshes)
	{
		mesh->Draw(gfx);
	}
}

void Vehicle::ImguiShow() noexcept
{
	switch (status)
	{
	case Vehicle::Status::FOLLOW_EARTH:
		ImGui::Text("Orbiting the Earth, press J change to transfer, press L to land");
		break;
	case Vehicle::Status::FOLLOW_MOON:
		ImGui::Text("Orbiting the Moon, press J change to transfer, press L to land");
		break;
	case Vehicle::Status::LANDING_EARTH:
		ImGui::Text("Landing on the Earth");
		break;
	case Vehicle::Status::LANDING_MOON:
		ImGui::Text("Landing on the Moon");
		break;
	case Vehicle::Status::ON_EARTH:
		ImGui::Text("On the Earth, press L to launch");
		break;
	case Vehicle::Status::ON_MOON:
		ImGui::Text("On the Moon, press L to launch");
		break;
	case Vehicle::Status::EARTH_LAUNCH:
	case Vehicle::Status::MOON_LAUNCH:
		ImGui::Text("Launching");
		break;
	case Vehicle::Status::FOLLOW_TRANS:
	{
		if (phi >= 0.0f)
			ImGui::Text("Orbiting the transfer, press J change to Moon");
		else
			ImGui::Text("Orbiting the transfer, press J change to Earth");
		break;
	}
	case Vehicle::Status::TRANS_TO_MOON:
	case Vehicle::Status::TRANS_TO_EARTH:
	case Vehicle::Status::MOON_TO_TRANS:
	case Vehicle::Status::EARTH_TO_TRANS:
		ImGui::Text("Waiting for orbit change, press Q to cancel");
		break;
	}
}


Mesh::Mesh(Vehicle& parent, const aiMesh& mesh, const aiMaterial* const* pMaterials, float scale, float& botBound)
	:
	parent(parent)
{
	namespace dx = DirectX;

	struct Vertex
	{
		dx::XMFLOAT3 pos;
		dx::XMFLOAT3 n;
		dx::XMFLOAT2 tc;
	};

	std::vector<Vertex> vertices;
	std::vector<unsigned short> indices;
	vertices.reserve(mesh.mNumVertices);
	for (unsigned int i = 0; i < mesh.mNumVertices; i++)
	{
		botBound = std::min(botBound, mesh.mVertices[i].y * scale);
		vertices.push_back({
			{ mesh.mVertices[i].x * scale,mesh.mVertices[i].y * scale,mesh.mVertices[i].z * scale },
			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i]),
			*reinterpret_cast<dx::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
			});
	}
	indices.reserve(mesh.mNumFaces * 3);
	for (unsigned int i = 0; i < mesh.mNumFaces; i++)
	{
		const auto& face = mesh.mFaces[i];
		assert(face.mNumIndices == 3);
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}

	if (mesh.mMaterialIndex >= 0)
	{
		auto& material = *pMaterials[mesh.mMaterialIndex];

		using namespace std::string_literals;
		aiString texFileName;
		material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName);
		AddBind( std::make_unique<Texture>(parent.gfx, Surface::FromFile("Models\\"s + texFileName.C_Str())) );
		// TODO: maybe there's no specular map
		material.GetTexture(aiTextureType_SPECULAR, 0, &texFileName);
		AddBind(std::make_unique<Texture>(parent.gfx, Surface::FromFile("Models\\"s + texFileName.C_Str()), 1u));

		AddBind(std::make_unique<Sampler>(parent.gfx));
	}

	AddBind(std::make_unique<VertexBuffer>(parent.gfx, vertices));

	AddIndexBuffer(std::make_unique<IndexBuffer>(parent.gfx, indices));

	auto pvs = std::make_unique<VertexShader>(parent.gfx, L"TexturedPhongVS.cso");
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));

	AddBind(std::make_unique<PixelShader>(parent.gfx, L"TexPhongSpecMapPS.cso"));

	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TexCoord",0,DXGI_FORMAT_R32G32_FLOAT,0,24,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};
	AddBind(std::make_unique<InputLayout>(parent.gfx, ied, pvsbc));

	AddBind(std::make_unique<Topology>(parent.gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBind(std::make_unique<TransformCbuf>(parent.gfx, *this));
}

void Mesh::Update(float dt) noexcept
{}

DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept
{
	return parent.GetTransformXM();
}

DirectX::XMMATRIX Mesh::GetFollowTransformXM() const noexcept
{
	return parent.GetTransformXM();
}
