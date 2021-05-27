Texture2D colorMap : register(t0);
Texture2D normalMap : register(t1);

SamplerState colorSampler : register(s0);

cbuffer cbChangerEveryFrame : register(b0)
{
	matrix worldMatrix;
};

cbuffer cbNeverChanges : register(b1)
{
	matrix viewMatrix;
};

cbuffer cbChangeOnResize : register(b2)
{
	matrix projMatrix;
};

cbuffer luces : register(b3)
{
	float4 luzAmbiental;
	float4 luzDifusa;
	float4 luzEspecular;
};
cbuffer factores : register(b4)
{
	float FAD;
	float FA;
	float FAS;
	float PAD;
};
cbuffer luzDirec : register(b5)
{
	float3 LuzDir;
	float Padd;
};

struct VS_Input
{
	float4 pos : POSITION;
	float2 tex0 : TEXCOORD0;
	float3 normal : NORMAL0;
	uint3  tangente  : TANGENT0;
};

struct PS_Input
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float3 normal : TEXCOORD1;
	float3 tangente : TEXCOORD2;
	float3 binormal : TEXCOORD3;
};

PS_Input VS_Main(VS_Input vertex)
{
	PS_Input vsOut = (PS_Input)0;
	vsOut.pos = mul(vertex.pos, worldMatrix);
	vsOut.pos = mul(vsOut.pos, viewMatrix);
	vsOut.pos = mul(vsOut.pos, projMatrix);

	vsOut.tex0 = vertex.tex0;


	vsOut.tangente = normalize(mul(vertex.tangente, worldMatrix));
	vsOut.normal = normalize(mul(vertex.normal, worldMatrix));
	vsOut.tangente = normalize(vsOut.tangente - vsOut.normal * dot(vsOut.normal, vsOut.tangente));
	vsOut.binormal = normalize(cross(vsOut.normal, vsOut.tangente));

	return vsOut;
}

float4 PS_Main(PS_Input pix) : SV_TARGET
{


	//float4 LuzAmbiental = float4(1.0, 1.0, 1.0, 1);


	///////////////////////////////////////////////
	////aqui se desarrolla el elemento ambiental  //
	///////////////////////////////////////////////
	//float4 text = colorMap.Sample(colorSampler, pix.tex0);
	//float FA = 0.3;
	//float4 AportAmb1 = LuzAmbiental * FA;

	////////////////////////////////////////////
	////aqui se desarrolla el elemento difuso //
	////////////////////////////////////////////
	//float3 DirLuz = float3(0.0f, 5.0f, 10.0f);
	//float4 LuzDifusa = float4(1, 1, 1, 1);
	//float FAD = 1;


	//float4 normal = normalMap.Sample(colorSampler, pix.tex0);
	//float3 bump = normalize(2.0 * normal - 1.0);
	//float3 dirLuzN = normalize(DirLuz);
	//float3x3 TBN = { pix.tangente, pix.binormal, pix.normal };
	//float3 bumpN = normalize(mul(normalize(bump), TBN));
	//float FALL = dot(bumpN, dirLuzN);

	//float4 AportDif = saturate(LuzDifusa * FALL * FAD);
	//

	//if (text.a < 0.1)
	//{
	//	clip(-1);
	//}

	//return text * AportDif;



	//float3 ambient = float3(0.1f, 0.1f, 0.1f);

	float4 text = colorMap.Sample(colorSampler, pix.tex0);
	//float intensity = 0.4;
	if (text.a < 0.1)
		//if(text.r >= intensity && text.g >= intensity && text.b >= intensity)
		{
			clip(-1);
			return 0;
		}
		else {
		//aqui se calcula la iluminacion difusa
		float4 textnorm = normalMap.Sample(colorSampler, pix.tex0);
		//float4 LuzAmbiental = float4(1.0, 1.0, 1.0, 1);
		//float FA = 0.3;
		///////////////////////////////////////////////
		// aqui se desarrolla el elemento ambiental
		///////////////////////////////////////////////
		float4 AportAmb = luzAmbiental * FA;
		//////////////////////////////////////////
		//aqui se desarrolla el elemento difuso //
		//////////////////////////////////////////
		//float3 DirLuz = float3(30, 10, 30);
		//float4 LuzDifusa = float4(1, 1, 1, 1);
		//float FAD = 1;

		float3 bump = normalize(2.0 * textnorm - 1.0);

		float FALL = dot(normalize(bump), normalize(LuzDir));
		float4 AportLuzDif = saturate(luzDifusa * FALL * FAD);
		text = text * (AportAmb + AportLuzDif);
		text.a = 1;
		return text;
	}

}