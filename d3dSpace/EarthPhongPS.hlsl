cbuffer LightCBuf
{
	float3 lightPos;
	float3 ambient;
	float3 diffuseColor;
	float diffuseIntensity;
	float attConst;
	float attLin;
	float attQuad;
};

Texture2D tex;
Texture2D nightTex;
SamplerState splr;


float4 main(float3 worldPos : Position, float3 n : Normal, float2 tc : TexCoord) : SV_Target
{
	//return tex.Sample(splr, tc);
	// fragment to light vector data
	const float3 vToL = lightPos - worldPos;
	const float distToL = length(vToL);
	const float3 dirToL = normalize(vToL / distToL);
	// attenuation
	const float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));
	// diffuse intensity
	if (dot(dirToL, n) < 0.0f)
	{
		return nightTex.Sample(splr, tc);
	}
	else 
	{
		const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToL, n));
		// final color
		return float4(saturate(diffuse + ambient), 1.0f) * tex.Sample(splr, tc);
	}
}
