cbuffer CBuf
{
	matrix modelView;
	matrix modelViewProj;
};

struct VSOut
{
	float2 tc : TexCoord;
	uint id : SurfId;
	float4 pos : SV_Position;
};

VSOut main(float3 pos : Position, float2 tex : Texcoord, uint id : Id)
{
	VSOut vso;
	vso.pos = mul(float4(pos, 1.0f), modelViewProj);
	vso.tc = tex;
	vso.id = id;
	return vso;
}
