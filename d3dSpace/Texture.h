#pragma once
#include "Bindable.h"

class Texture : public Bindable
{
public:
	Texture(Graphics& gfx, const class Surface& s, UINT slot = 0u);
	void Bind(Graphics& gfx) noexcept override;
protected:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
private:
	UINT slot;
};