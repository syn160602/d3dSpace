#pragma once
#include "Drawable.h"
#include "myMath.h"

class SkinnedBox : public Drawable
{
public:
	SkinnedBox(Graphics& gfx, float scale);
	void Update(float dt) noexcept
	{}
	DirectX::XMMATRIX GetTransformXM() const noexcept;
	DirectX::XMMATRIX GetFollowTransformXM() const noexcept;
private:
	const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept override
	{
		return {};
	}
private:
	float scale;
};