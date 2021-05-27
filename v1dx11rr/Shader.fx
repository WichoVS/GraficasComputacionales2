Texture2D colorMap1 : register(t0);
Texture2D colorMap2 : register(t1);
Texture2D colorMap3 : register(t2);

Texture2D specularMap1 : register(t3);
Texture2D specularMap2 : register(t4);
Texture2D specularMap3 : register(t5);

Texture2D normalMap1 : register(t6);
Texture2D normalMap2 : register(t7);
Texture2D normalMap3 : register(t8);

Texture2D blendMap : register(t9);

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
	float2 blendTex : TEXCOORD1;
	float3 normal : NORMAL0;
	float3 tangente : NORMAL1;
	float3 binormal : NORMAL2;
};

struct PS_Input
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float2 blendTex : TEXCOORD1;
	float3 normal : NORMAL0;
	float3 tangent : NORMAL1;
	float3 binorm : NORMAL2;
};

PS_Input VS_Main(VS_Input vertex)
{
	PS_Input vsOut = (PS_Input)0;
	vsOut.pos = mul(vertex.pos, worldMatrix);
	vsOut.pos = mul(vsOut.pos, viewMatrix);
	vsOut.pos = mul(vsOut.pos, projMatrix);

	vsOut.tex0 = vertex.tex0;
	vsOut.blendTex = vertex.blendTex;
	vsOut.normal = normalize(mul(vertex.normal, worldMatrix));
	vsOut.tangent = normalize(mul(vertex.tangente, worldMatrix));
	vsOut.binorm = normalize(mul(vertex.binormal, worldMatrix));

	return vsOut;
}

float4 PS_Main(PS_Input pix) : SV_TARGET
{
	float4 fColor = float4(1,0,0,1);

	//float4 LuzAmbiental = float4(1.0, 1.0, 1.0, 1);


	///////////////////////////////////////////////
	// aqui se desarrolla el elemento ambiental  //
	///////////////////////////////////////////////
	float4 color1 = colorMap1.Sample(colorSampler, pix.tex0);
	//float FA = 0.3;
	float4 AportAmb1 = luzAmbiental * FA;
	
	//////////////////////////////////////////
	//aqui se desarrolla el elemento difuso //
	//////////////////////////////////////////
	//float3 DirLuz = float3(0.0f, 5.0f, 10.0f);
	//float4 LuzDifusa = float4(1, 1, 1, 1);
	//float FAD = 1;


	float4 normal1 = normalMap1.Sample(colorSampler, pix.tex0);
	float3 bump1 = normalize(2.0 * normal1 - 1.0);
	float3 dirLuzN = normalize(LuzDir);
	float3x3 TBN = { pix.tangent, pix.binorm, pix.normal };
	float3 bumpN1 = normalize(mul(normalize(bump1), TBN));
	float FALL1 = dot(bumpN1, dirLuzN);

	float4 AportDif1 = saturate(luzDifusa * FALL1 * FAD);


	//////////////////////////////////////////////////////
	// aqui se desarrolla el elemento especular
	//////////////////////////////////////////////////////
	//float4 luzEspecular = { 1.0f, 1.0f, 1.0f, 1.0f };
	float4 specular1 = specularMap1.Sample(colorSampler, pix.tex0);
	float3 reflejo = normalize(2 * FALL1 * bumpN1 - dirLuzN);
	//float FAS = 3.0f;
	float4 AportEspec = pow(saturate(dot(reflejo, dirLuzN)), 50) * FAS * luzEspecular;
	float4 aportE1 = specular1 * luzEspecular * FAS * AportEspec;

	float4 textura1 = color1 * (AportAmb1 + AportDif1 + aportE1);





	///////////////////////////////////////////////
	// aqui se desarrolla el elemento ambiental  //
	///////////////////////////////////////////////
	float4 color2 = colorMap2.Sample(colorSampler, pix.tex0);
	float4 AportAmb2 = luzAmbiental * FA;

	//////////////////////////////////////////
	//aqui se desarrolla el elemento difuso //
	//////////////////////////////////////////
	float4 normal2 = normalMap2.Sample(colorSampler, pix.tex0);
	float3 bump2 = normalize(2.0 * normal2 - 1.0);
	float3 bumpN2 = normalize(mul(normalize(bump2), TBN));
	float FALL2 = dot(bumpN2, dirLuzN);

	float4 AportDif2 = saturate(luzDifusa * FALL2 * FAD);

	float4 specular2 = specularMap2.Sample(colorSampler, pix.tex0);
	float3 reflejo2 = normalize(2 * FALL2 * bumpN2 - dirLuzN);
	float4 AportEspec2 = pow(saturate(dot(reflejo2, dirLuzN)), 50) * FAS * luzEspecular;
	float4 aportE2 = specular2 * luzEspecular * FAS * AportEspec2;

	float4 textura2 = color2 * (AportAmb2 + AportDif2 + aportE2);







	///////////////////////////////////////////////
	// aqui se desarrolla el elemento ambiental  //
	///////////////////////////////////////////////
	float4 color3 = colorMap3.Sample(colorSampler, pix.tex0);
	float4 AportAmb3 = luzAmbiental * FA;

	//////////////////////////////////////////
	//aqui se desarrolla el elemento difuso //
	//////////////////////////////////////////
	float4 normal3 = normalMap3.Sample(colorSampler, pix.tex0);
	float3 bump3 = normalize(2.0 * normal3 - 1.0);
	float3 bumpN3 = normalize(mul(normalize(bump3), TBN));
	float FALL3 = dot(bumpN3, dirLuzN);

	float4 AportDif3 = saturate(luzDifusa * FALL3 * FAD);

	float4 specular3 = specularMap3.Sample(colorSampler, pix.tex0);
	float3 reflejo3 = normalize(2 * FALL3 * bumpN3 - dirLuzN);
	float4 AportEspec3 = pow(saturate(dot(reflejo3, dirLuzN)), 50) * FAS * luzEspecular;
	float4 aportE3 = specular3 * luzEspecular * FAS * AportEspec3;

	float4 textura3 = color3 * (AportAmb3 + AportDif3 + aportE3);

	float4 blend = blendMap.Sample(colorSampler, pix.blendTex);

	float4 mixG = lerp(textura2, textura1, blend.g);
	mixG = lerp(mixG, textura3, blend.b);
	fColor = mixG;

	return fColor;
}