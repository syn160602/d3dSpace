// Texture2D tex;
Texture2D tex0 : register(t0);
Texture2D tex1 : register(t1);
Texture2D tex2 : register(t2);
Texture2D tex3 : register(t3);
Texture2D tex4 : register(t4);
Texture2D tex5 : register(t5);

SamplerState splr;

float4 main(float2 tc : TexCoord, uint id : SurfId) : SV_TARGET
{
	float4 ret;
	if (id == 0)
	{
		ret = tex0.Sample(splr, tc);
	}
	else if (id == 1)
	{
		ret = tex1.Sample(splr, tc);
	}
	else if (id == 2)
	{
		ret = tex2.Sample(splr, tc);

	}
	else if (id == 3)
	{
		ret = tex3.Sample(splr, tc);
	}
	else if (id == 4)
	{
		ret = tex4.Sample(splr, tc);
	}
	else
	{
		ret = tex5.Sample(splr, tc);
	}
	return ret * 0.5f;
}
