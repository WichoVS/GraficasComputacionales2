Texture2D textures : register(t0);
Texture2D textures2 : register(t1);
Texture2D textures3 : register(t2);

SamplerState colorSampler : register(s0);

cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projMatrix;
	float4 valores;
};

struct VS_Input
{
	float4 pos : POSITION;
	float2 tex0 : TEXCOORD0;
};

struct PS_Input
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float4 var : TEXCOORD1;
};

PS_Input VS_Main(VS_Input vertex)
{
	PS_Input vsOut = (PS_Input)0;
	vsOut.pos = mul(vertex.pos, worldMatrix);
	vsOut.pos = mul(vsOut.pos, viewMatrix);
	vsOut.pos = mul(vsOut.pos, projMatrix);

	vsOut.tex0 = vertex.tex0;
	vsOut.var = valores;

	return vsOut;
}

float4 PS_Main(PS_Input pix) : SV_TARGET
{
	float4 textMF;
	//float4 outputColor = float4(1,1,1,1);
	float4 Dia = textures.Sample(colorSampler, pix.tex0);
	float4 Nublado = textures2.Sample(colorSampler, pix.tex0);
	float4 Noche = textures3.Sample(colorSampler, pix.tex0);



	if (pix.var.w < 80)
		textMF = lerp(Nublado,Dia, saturate(pix.var.w/80));

	if (pix.var.w < 160 && pix.var.w > 79)
		textMF = lerp(Dia, Noche, saturate((pix.var.w - 80) / 80));

	if (pix.var.w > 159)
		textMF = lerp(Noche, Nublado, saturate((pix.var.w - 120) / 40));

	float4 outputColor = textMF;

	return outputColor;
}