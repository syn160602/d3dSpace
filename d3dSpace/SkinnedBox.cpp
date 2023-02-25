#include "SkinnedBox.h"
#include "BindableBase.h"
#include "GraphicThrowMacros.h"
#include "Cube.h"
#include "Surface.h"
#include "Texture.h"
#include "Sampler.h"


SkinnedBox::SkinnedBox(Graphics& gfx, float scale)
	:
	scale(scale)
{
	namespace dx = DirectX;

	struct Vertex
	{
		dx::XMFLOAT3 pos;
		dx::XMFLOAT2 tc;
		unsigned int id;
	};
	auto model = Cube::MakeIndependentTextured<Vertex>();

	AddBind(std::make_unique<VertexBuffer>(gfx, model.vertices));

	AddBind(std::make_unique<Texture>(gfx, Surface::FromFile("Images\\galaxy\\galaxy-Z.jpg")));
	AddBind(std::make_unique<Texture>(gfx, Surface::FromFile("Images\\galaxy\\galaxy+Z.jpg"), 1u));
	AddBind(std::make_unique<Texture>(gfx, Surface::FromFile("Images\\galaxy\\galaxy-X.jpg"), 2u));
	AddBind(std::make_unique<Texture>(gfx, Surface::FromFile("Images\\galaxy\\galaxy+X.jpg"), 3u));
	AddBind(std::make_unique<Texture>(gfx, Surface::FromFile("Images\\galaxy\\galaxy-Y.jpg"), 4u));
	AddBind(std::make_unique<Texture>(gfx, Surface::FromFile("Images\\galaxy\\galaxy+Y.jpg"), 5u));

	AddBind(std::make_unique<Sampler>(gfx));

	auto pvs = std::make_unique<VertexShader>(gfx, L"TexBgVS.cso");
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));

	AddBind(std::make_unique<PixelShader>(gfx, L"TexBgPS.cso"));

	AddIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.indices));

	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TexCoord",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "Id",0,DXGI_FORMAT_R32_UINT,0,20,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};
	AddBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));

	AddBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBind(std::make_unique<TransformCbuf>(gfx, *this));
}

DirectX::XMMATRIX SkinnedBox::GetTransformXM() const noexcept
{
	namespace dx = DirectX;
	return dx::XMMatrixScaling(scale, scale, scale);
}

DirectX::XMMATRIX SkinnedBox::GetFollowTransformXM() const noexcept
{
	namespace dx = DirectX;
	return dx::XMMatrixScaling(scale, scale, scale);
}
