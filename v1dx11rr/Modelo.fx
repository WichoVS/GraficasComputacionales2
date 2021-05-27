Texture2D colorMap : register(t0);
Texture2D normalMap : register(t1);
Texture2D specularMap: register(t2);

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

cbuffer gener : register(b3)
{
	float4 generales;
};

cbuffer cam : register(b4)
{
	float4 camara;
};

cbuffer luces : register(b5)
{
	float4 luzAmbiental;
	float4 luzDifusa;
	float4 luzEspecular;
};
cbuffer factores : register(b6)
{
	float FAD;
	float FA;
	float FAS;
	float PAD;
};
cbuffer luzDirec : register(b7)
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
	float3 generais : TEXCOORD4;
	float4 dirN : TEXCOORD5;

};

PS_Input VS_Main(VS_Input vertex)
{
	PS_Input vsOut = (PS_Input)0;

	//float2 texCoords = vertex.tex0 / 5;
	//float4 alturaMap = specularMap.SampleLevel(colorSamplerVer, texCoords,0 );
	////float4 colors = colorMap.SampleLevel(colorSampler, float4(position.x * 0.001, position.z * 0.001, 0, 0), 0);
	//float4 newPos = vertex.pos + (float4(vertex.normal,0) * 10 * alturaMap.r);


	vsOut.pos = mul(vertex.pos, worldMatrix);
	vsOut.pos = mul(vsOut.pos, viewMatrix);
	vsOut.pos = mul(vsOut.pos, projMatrix);

	//vsOut.dirN = (DirLuz - vsOut.pos);

	vsOut.tex0 = vertex.tex0;

	vsOut.tangente = normalize(mul(vertex.tangente, worldMatrix));
	vsOut.normal = normalize(mul(vertex.normal, worldMatrix));
	vsOut.tangente = normalize(vsOut.tangente - vsOut.normal * dot(vsOut.normal, vsOut.tangente));
	vsOut.binormal = normalize(cross(vsOut.normal, vsOut.tangente));

	vsOut.generais = generales;

	return vsOut;
}

float4 PS_Main(PS_Input pix) : SV_TARGET
{
	/////////////////////////////////////////////
	// aqui se desarrolla el elemento ambiental
	/////////////////////////////////////////////
	//float4 LuzAmbiental = float4(1.0, 1.0, 1.0, 1);
	//float FA = 0.1;
	float4 textColor = colorMap.Sample(colorSampler, pix.tex0);
	float4 AportAmb = luzAmbiental * FA;

	//////////////////////////////////////////
	//aqui se desarrolla el elemento difuso //
	//////////////////////////////////////////
	//float3 DirLuz = float3(0.0f, 5.0f, 10.0f);
	//float4 LuzDifusa = float4(1, 1, 1, 1);
	//float FAD = 0.8;



	float4 textNorm = normalMap.Sample(colorSampler, pix.tex0);
	float3 bump = normalize(2.0 * textNorm - 1.0);
	float3 dirLuzN = normalize(LuzDir);
	float3x3 TBN = {pix.tangente, pix.binormal, pix.normal};
	float3 bumpN = normalize(mul(normalize(bump), TBN));
	float FALL = dot(bumpN, dirLuzN);

	float4 AportLuzDif = saturate(luzDifusa * FALL * FAD);

	//////////////////////////////////////////////////////
	// aqui se desarrolla el elemento especular
	//////////////////////////////////////////////////////
	//float4 luzEspecular = { 1.0f, 1.0f, 1.0f, 1.0f };
	float4 textSpec = specularMap.Sample(colorSampler, pix.tex0);
	float3 reflejo = normalize(2 * FALL * bumpN - dirLuzN);
	//float FAS = 3.0f;
	float4 AportEspec = pow(saturate(dot(reflejo, dirLuzN)), 50) * FAS * luzEspecular;
	float4 aportE = textSpec * luzEspecular * FAS * AportEspec;
	//float4 aportE = luzEspecular * FAS * AportEspec;

	textColor = textColor * (AportAmb + AportLuzDif + aportE);

	//textColor = textColor * (AportAmb + AportLuzDif);

	return textColor;
}