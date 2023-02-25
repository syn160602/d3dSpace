cbuffer CBuf
{
	matrix modelView;
	matrix modelViewProj;
};

struct VSOut
{
	float2 tc : TexCoord;
	float4 pos : SV_Position;
};

VSOut main(float3 pos : Position, float2 tex : Texcoord)
{
	VSOut vso;
	vso.pos = mul(float4(pos, 1.0f), modelViewProj);
	vso.tc = tex;
	return vso;
}
