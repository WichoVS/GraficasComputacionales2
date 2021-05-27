#ifndef _modelo
#define _modelo

//#define _XM_NO_INTRINSICS_

#include <d3d11.h>
#include <d3dx11.h>
#include <DxErr.h>
#include <D3Dcompiler.h>
#include <d3dx10math.h>
#include <conio.h>
#include <vector>
#include <iostream>
#include "FuncionesGC.h"

using namespace std;

class ModeloRR {
private:
	struct VertexComponent
	{
		D3DXVECTOR3 pos;
		D3DXVECTOR2 UV;
		D3DXVECTOR3 normal;
		D3DXVECTOR3 tangente;
	};

	struct VertexCollide
	{
		D3DXVECTOR3 pos;
	};

	struct vector3 {
		float x, y, z;
	};

	struct vector2 {
		float u, v;
	};

	struct Luces
	{
		D3DXVECTOR4 luzAmbiental;
		D3DXVECTOR4 luzDifusa;
		D3DXVECTOR4 luzEspecular;
	};

	struct Factores
	{
		float FAD;
		float FA;
		float FAS;
		float PAD;
	};

	struct LuzDire
	{
		D3DXVECTOR3 LuzDir;
		float Padd;
	};

	ID3D11VertexShader* VertexShaderVS;
	ID3D11PixelShader* solidColorPS;

	ID3D11InputLayout* inputLayout;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	ID3D11ShaderResourceView* colorMap;
	ID3D11ShaderResourceView* normalMap;
	ID3D11ShaderResourceView* specularMap;
	ID3D11SamplerState* colorMapSampler;

	ID3D11Buffer* viewCB;
	ID3D11Buffer* projCB;
	ID3D11Buffer* worldCB;
	ID3D11Buffer* generales;
	ID3D11Buffer* camara;
	ID3D11Buffer* luces;
	ID3D11Buffer* factores;
	ID3D11Buffer* dirLuz;

	D3DXMATRIX viewMatrix;
	D3DXMATRIX projMatrix;

	ID3D11BlendState* Transparency;

	int ancho, alto;
	int anchoTexTerr, altoTexTerr;
	float anchof, altof;
	float deltax, deltay;
	float escala;
	float pX, pY, pZ;

	UINT* indices;
	int cantind;
	VertexComponent* vertices;
	int cantvert;
	VertexCollide* vertcol;
	ID3D11Device* d3dDevice;
	ID3D11DeviceContext* d3dContext;


public:
	//float X, float Y, float Z
	ModeloRR(const char* modelPath, WCHAR* diffuseTex, WCHAR* normaltex, WCHAR* speculartex, ID3D11Device* D3DDevice, ID3D11DeviceContext* D3DContext, float X, float Y, float Z,float scale)
	{
		//copiamos el device y el device context a la clase terreno
		d3dContext = D3DContext;
		d3dDevice = D3DDevice;
		escala = scale;
		pX = X;
		pY = Y;
		pZ = Z;
		//aqui cargamos las texturas de alturas y el cesped
		CargaParametros(diffuseTex, normaltex, speculartex, modelPath);
	}

	~ModeloRR()
	{
		//libera recursos
		delete vertcol;
		UnloadContent();
	}

	bool CompileD3DShader(WCHAR* filePath, char* entry, char* shaderModel, ID3DBlob** buffer)
	{
		//forma de compilar el shader
		DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

		ID3DBlob* errorBuffer = 0;
		HRESULT result;

		result = D3DX11CompileFromFile(filePath, 0, 0, entry, shaderModel, shaderFlags,
			0, 0, buffer, &errorBuffer, 0);
		if (FAILED(result))
		{
			if (errorBuffer != 0)
			{
				OutputDebugStringA((char*)errorBuffer->GetBufferPointer());
				errorBuffer->Release();
			}
			return false;
		}

		if (errorBuffer != 0)
			errorBuffer->Release();

		return true;
	}

	bool CargaParametros(WCHAR* diffuseTex, WCHAR* normaltex, WCHAR* speculartex, const char* modelPath)
	{
		HRESULT d3dResult;

		ID3DBlob* vsBuffer = 0;

		//cargamos el shaders de vertices que esta contenido en el Shader.fx, note
		//que VS_Main es el nombre del vertex shader en el shader, vsBuffer contendra
		//al puntero del mismo
		bool compileResult = CompileD3DShader(L"Modelo.fx", "VS_Main", "vs_4_0", &vsBuffer);
		//en caso de no poder cargarse ahi muere la cosa
		if (compileResult == false)
		{
			return false;
		}

		//aloja al shader en la memeoria del gpu o el cpu
		d3dResult = d3dDevice->CreateVertexShader(vsBuffer->GetBufferPointer(),
			vsBuffer->GetBufferSize(), 0, &VertexShaderVS);
		//en caso de falla sale
		if (FAILED(d3dResult))
		{

			if (vsBuffer)
				vsBuffer->Release();

			return false;
		}

		//lo nuevo, antes creabamos una estructura con la definicion del vertice, ahora creamos un mapa o layout
		//de como queremos construir al vertice, esto es la definicion
		D3D11_INPUT_ELEMENT_DESC solidColorLayout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		unsigned int totalLayoutElements = ARRAYSIZE(solidColorLayout);

		d3dResult = d3dDevice->CreateInputLayout(solidColorLayout, totalLayoutElements,
			vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), &inputLayout);

		vsBuffer->Release();

		if (FAILED(d3dResult))
		{
			return false;
		}

		ID3DBlob* psBuffer = 0;
		// de los vertices pasamos al pixel shader, note que el nombre del shader es el mismo
		//ahora buscamos al pixel shader llamado PS_Main
		compileResult = CompileD3DShader(L"Modelo.fx", "PS_Main", "ps_4_0", &psBuffer);

		if (compileResult == false)
		{
			return false;
		}
		//ya compilado y sin error lo ponemos en la memoria
		d3dResult = d3dDevice->CreatePixelShader(psBuffer->GetBufferPointer(),
			psBuffer->GetBufferSize(), 0, &solidColorPS);

		psBuffer->Release();

		if (FAILED(d3dResult))
		{
			return false;
		}

		//aqui va la carga del modelo con el metodo creado
		CargaModelos(modelPath);

		//proceso de guardar el buffer de vertices para su uso en el render
		D3D11_BUFFER_DESC vertexDesc;
		ZeroMemory(&vertexDesc, sizeof(vertexDesc));
		vertexDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexDesc.ByteWidth = sizeof(VertexComponent) * cantvert;

		D3D11_SUBRESOURCE_DATA resourceData;
		ZeroMemory(&resourceData, sizeof(resourceData));
		resourceData.pSysMem = vertices;

		d3dResult = d3dDevice->CreateBuffer(&vertexDesc, &resourceData, &vertexBuffer);

		if (FAILED(d3dResult))
		{
			MessageBox(0, L"Error", L"Error al crear vertex buffer", MB_OK);
			return false;
		}
		//ya una vez pasados los vertices al buffer borramos el arreglo donde los habiamos creado inicialmente
		delete vertices;

		D3D11_BUFFER_DESC indexDesc;
		ZeroMemory(&indexDesc, sizeof(indexDesc));
		indexDesc.Usage = D3D11_USAGE_DEFAULT;
		indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexDesc.ByteWidth = sizeof(INT) * cantind;
		indexDesc.CPUAccessFlags = 0;

		resourceData;
		ZeroMemory(&resourceData, sizeof(resourceData));
		resourceData.pSysMem = indices;

		d3dResult = d3dDevice->CreateBuffer(&indexDesc, &resourceData, &indexBuffer);

		if (FAILED(d3dResult))
		{
			return false;
		}
		delete indices;

		//crea los accesos de las texturas para los shaders 
		d3dResult = D3DX11CreateShaderResourceViewFromFile(d3dDevice, diffuseTex, 0, 0, &colorMap, 0);

		if (FAILED(d3dResult))
		{
			return false;
		}

		//crea los accesos de las texturas para los shaders 
		d3dResult = D3DX11CreateShaderResourceViewFromFile(d3dDevice, normaltex, 0, 0, &normalMap, 0);

		if (FAILED(d3dResult))
		{
			return false;
		}

		//crea los accesos de las texturas para los shaders 
		d3dResult = D3DX11CreateShaderResourceViewFromFile(d3dDevice, speculartex, 0, 0, &specularMap, 0);

		if (FAILED(d3dResult))
		{
			return false;
		}
		//aqui creamos el sampler
		D3D11_SAMPLER_DESC colorMapDesc;
		ZeroMemory(&colorMapDesc, sizeof(colorMapDesc));
		colorMapDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		colorMapDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		colorMapDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		colorMapDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		colorMapDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		colorMapDesc.MaxLOD = D3D11_FLOAT32_MAX;
		//con la estructura de descripion creamos el sampler
		d3dResult = d3dDevice->CreateSamplerState(&colorMapDesc, &colorMapSampler);

		if (FAILED(d3dResult))
		{
			return false;
		}

		//creamos los buffers para el shader para poder pasarle las matrices
		D3D11_BUFFER_DESC constDesc;
		ZeroMemory(&constDesc, sizeof(constDesc));
		constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constDesc.ByteWidth = sizeof(D3DXMATRIX);
		constDesc.Usage = D3D11_USAGE_DEFAULT;
		//de vista
		d3dResult = d3dDevice->CreateBuffer(&constDesc, 0, &viewCB);

		if (FAILED(d3dResult))
		{
			return false;
		}
		//de proyeccion
		d3dResult = d3dDevice->CreateBuffer(&constDesc, 0, &projCB);

		if (FAILED(d3dResult))
		{
			return false;
		}
		//de mundo
		d3dResult = d3dDevice->CreateBuffer(&constDesc, 0, &worldCB);

		if (FAILED(d3dResult))
		{
			return false;
		}

		//creamos los buffers para el shader para poder pasarle las matrices
		D3D11_BUFFER_DESC constDesc2;
		ZeroMemory(&constDesc2, sizeof(constDesc2));
		constDesc2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constDesc2.ByteWidth = sizeof(D3DXVECTOR4);
		constDesc2.Usage = D3D11_USAGE_DEFAULT;

		d3dResult = d3dDevice->CreateBuffer(&constDesc2, 0, &generales);


		//creamos los buffers para el shader para poder pasarle las matrices
		D3D11_BUFFER_DESC constDesc3;
		ZeroMemory(&constDesc3, sizeof(constDesc3));
		constDesc3.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constDesc3.ByteWidth = sizeof(D3DXVECTOR4);
		constDesc3.Usage = D3D11_USAGE_DEFAULT;

		d3dResult = d3dDevice->CreateBuffer(&constDesc3, 0, &camara);

		D3D11_BUFFER_DESC constDesc4;
		ZeroMemory(&constDesc4, sizeof(constDesc4));
		constDesc4.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constDesc4.ByteWidth = sizeof(Luces);
		constDesc4.Usage = D3D11_USAGE_DEFAULT;

		d3dResult = d3dDevice->CreateBuffer(&constDesc4, 0, &luces);

		D3D11_BUFFER_DESC constDesc5;
		ZeroMemory(&constDesc5, sizeof(constDesc5));
		constDesc5.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constDesc5.ByteWidth = sizeof(Factores);
		constDesc5.Usage = D3D11_USAGE_DEFAULT;

		d3dResult = d3dDevice->CreateBuffer(&constDesc5, 0, &factores);

		D3D11_BUFFER_DESC constDesc6;
		ZeroMemory(&constDesc6, sizeof(constDesc6));
		constDesc6.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constDesc6.ByteWidth = sizeof(LuzDire);
		constDesc6.Usage = D3D11_USAGE_DEFAULT;

		d3dResult = d3dDevice->CreateBuffer(&constDesc6, 0, &dirLuz);

		if (FAILED(d3dResult))
		{
			return false;
		}

		D3D11_BLEND_DESC blendDesc;
		ZeroMemory(&blendDesc, sizeof(blendDesc));

		D3D11_RENDER_TARGET_BLEND_DESC rtbd;
		ZeroMemory(&rtbd, sizeof(rtbd));

		rtbd.BlendEnable = true;
		rtbd.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		rtbd.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		rtbd.BlendOp = D3D11_BLEND_OP_ADD;
		rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
		rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
		rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		rtbd.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

		blendDesc.AlphaToCoverageEnable = false;
		blendDesc.RenderTarget[0] = rtbd;

		//d3dDevice->CreateBlendState(&blendDesc, &Transparency);

		//posicion de la camara
		D3DXVECTOR3 eye = D3DXVECTOR3(0.0f, 100.0f, 200.0f);
		//a donde ve
		D3DXVECTOR3 target = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

		//crea la matriz de vista
		D3DXMatrixLookAtLH(&viewMatrix, &eye, &target, &up);
		//la de proyeccion
		D3DXMatrixPerspectiveFovLH(&projMatrix, D3DX_PI / 4.0, ancho / alto, 0.01f, 1000.0f);
		//las transpone para acelerar la multiplicacion
		D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
		D3DXMatrixTranspose(&projMatrix, &projMatrix);

		return true;
	}

	bool UnloadContent()
	{
		if (colorMapSampler)
			colorMapSampler->Release();
		if (colorMap)
			colorMap->Release();
		if (VertexShaderVS)
			VertexShaderVS->Release();
		if (solidColorPS)
			solidColorPS->Release();
		if (inputLayout)
			inputLayout->Release();
		if (vertexBuffer)
			vertexBuffer->Release();
		if (viewCB)
			viewCB->Release();
		if (projCB)
			projCB->Release();
		if (worldCB)
			worldCB->Release();
		if (Transparency)
			Transparency->Release();


		colorMapSampler = 0;
		colorMap = 0;
		VertexShaderVS = 0;
		solidColorPS = 0;
		inputLayout = 0;
		vertexBuffer = 0;
		indexBuffer = 0;
		viewCB = 0;
		projCB = 0;
		worldCB = 0;
	}

	void Update(float dt)
	{
	}

	void Draw(D3DXMATRIX vista, D3DXMATRIX proyeccion, float rotation, D3DXVECTOR4 camaraPos, D3DXVECTOR4 ambiental, D3DXVECTOR4 difusa, D3DXVECTOR4 especular
		, float fad, float fa, float fas, D3DXVECTOR3 luzdir)
	{


		//paso de datos, es decir cuanto es el ancho de la estructura
		unsigned int stride = sizeof(VertexComponent);
		unsigned int offset = 0;

		//define la estructura del vertice a traves de layout
		d3dContext->IASetInputLayout(inputLayout);

		//define con que buffer trabajara
		d3dContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		//define con buffer de indices trabajara
		d3dContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		//define la forma de conexion de los vertices
		d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		//d3dContext->OMSetBlendState(Transparency, blendFactor, 0xffffffff);
		//Establece el vertex y pixel shader que utilizara
		d3dContext->VSSetShader(VertexShaderVS, 0, 0);
		d3dContext->PSSetShader(solidColorPS, 0, 0);
		//pasa lo sbuffers al shader
		d3dContext->PSSetShaderResources(0, 1, &colorMap);
		d3dContext->PSSetShaderResources(1, 1, &normalMap);
		d3dContext->VSSetShaderResources(2, 1, &specularMap);
		d3dContext->PSSetSamplers(0, 1, &colorMapSampler);

		//mueve la camara
		D3DXMATRIX rotationMat;
		D3DXMatrixRotationYawPitchRoll(&rotationMat, rotation, 0.0f, 0.0f);
		D3DXMATRIX translationMat;
		D3DXMatrixTranslation(&translationMat, pX, pY, pZ);
		D3DXMATRIX ry;
		D3DXMatrixRotationY(&ry, rotation);
		viewMatrix *= ry;

		D3DXMATRIX worldMat = rotationMat * translationMat;//= rotationMat * translationMat;
		D3DXMatrixTranspose(&worldMat, &worldMat);
		//actualiza los buffers del shader
		d3dContext->UpdateSubresource(worldCB, 0, 0, &worldMat, 0, 0);
		d3dContext->UpdateSubresource(viewCB, 0, 0, &vista, 0, 0);
		d3dContext->UpdateSubresource(projCB, 0, 0, &proyeccion, 0, 0);

		D3DXVECTOR4 aux;
		aux.x = 0.0;
		aux.y = 5.0;
		aux.z = 10.0;

		Luces auxLuces;
		auxLuces.luzAmbiental = ambiental;
		auxLuces.luzDifusa = difusa;
		auxLuces.luzEspecular = especular;

		Factores auxFactores;
		auxFactores.FA = fa;
		auxFactores.FAD = fad;
		auxFactores.FAS = fas;
		auxFactores.PAD = 0.0f;

		LuzDire auxDirLuz;
		auxDirLuz.LuzDir = luzdir;
		auxDirLuz.Padd = 0.0f;


		//auxFactores.FAD = auxDirLuz.LuzDir.y;
		//if (auxFactores.FAD <= 80)
		//	auxFactores.FAD = 80;
		//auxFactores.FAD = auxFactores.FAD / 350;

		auxLuces.luzDifusa = D3DXVECTOR4(1, clamp(auxFactores.FAD, 0.6f, 1.0f), clamp(auxFactores.FAD, 0.6f, 1.0f), 1);
		auxLuces.luzAmbiental = D3DXVECTOR4(1, clamp(auxFactores.FAD, 0.6f, 1.0f), clamp(auxFactores.FAD, 0.6f, 1.0f), 1);
		auxLuces.luzEspecular = D3DXVECTOR4(1, clamp(auxFactores.FAD, 0.6f, 1.0f), clamp(auxFactores.FAD, 0.6f, 1.0f), 1);


		d3dContext->UpdateSubresource(generales, 0, 0, &aux, 0, 0);
		d3dContext->UpdateSubresource(camara, 0, 0, &camaraPos, 0, 0);
		d3dContext->UpdateSubresource(luces, 0, 0, &auxLuces, 0, 0);
		d3dContext->UpdateSubresource(factores, 0, 0, &auxFactores, 0, 0);
		d3dContext->UpdateSubresource(dirLuz, 0, 0, &auxDirLuz, 0, 0);




		//le pasa al shader los buffers
		d3dContext->VSSetConstantBuffers(0, 1, &worldCB);
		d3dContext->VSSetConstantBuffers(1, 1, &viewCB);
		d3dContext->VSSetConstantBuffers(2, 1, &projCB);
		d3dContext->VSSetConstantBuffers(3, 1, &generales);
		d3dContext->VSSetConstantBuffers(4, 1, &camara);
		d3dContext->PSSetConstantBuffers(5, 1, &luces);
		d3dContext->PSSetConstantBuffers(6, 1, &factores);
		d3dContext->PSSetConstantBuffers(7, 1, &dirLuz);
		//cantidad de trabajos

		d3dContext->DrawIndexed(cantind, 0, 0);


	}


	void CargaModelos(const char* modelPath)
	{
		vector<vector3> verticesM;
		vector<vector2> uvs;
		vector<vector3> normales;
		vector<int> vertexIndices;
		vector<int> uvIndices;
		vector<int> normalIndices;

		errno_t err;
		FILE* file;
		err = fopen_s(&file, modelPath , "r");
		if (file == NULL) {
			printf("Impossible to open the file !\n");
			return;
		}

		while (1)
		{
			char lineHeader[128];
			// Lee la primera palabra de la línea
			int res = fscanf_s(file, "%s", lineHeader, 128);
			if (res == EOF)
				break; // EOF = End Of File, es decir, el final del archivo. Se finaliza el ciclo.

			if (strcmp(lineHeader, "v") == 0)
			{
				vector3 vertex;
				fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
				verticesM.push_back(vertex);
			}
			else if (strcmp(lineHeader, "vt") == 0)
			{
				vector2 uv;
				fscanf_s(file, "%f %f\n", &uv.u, &uv.v);
				uvs.push_back(uv);
			}
			else if (strcmp(lineHeader, "vn") == 0) {
				vector3 normal;
				fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
				normales.push_back(normal);
			}
			else if (strcmp(lineHeader, "f") == 0) {
				std::string vertex1, vertex2, vertex3;
				unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
				int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
					&vertexIndex[0], &uvIndex[0], &normalIndex[0],
					&vertexIndex[1], &uvIndex[1], &normalIndex[1],
					&vertexIndex[2], &uvIndex[2], &normalIndex[2]);
				if (matches != 9) {
					printf("File can't be read by our simple parser : ( Try exporting with other options\n");
					return;
				}
				vertexIndices.push_back(vertexIndex[0]);
				vertexIndices.push_back(vertexIndex[1]);
				vertexIndices.push_back(vertexIndex[2]);
				uvIndices.push_back(uvIndex[0]);
				uvIndices.push_back(uvIndex[1]);
				uvIndices.push_back(uvIndex[2]);
				normalIndices.push_back(normalIndex[0]);
				normalIndices.push_back(normalIndex[1]);
				normalIndices.push_back(normalIndex[2]);
			}
		}

		vector<VertexComponent> tempoVertices;
		vector<unsigned int> tempoIndices;

		for (int i = 0; i < vertexIndices.size(); i++)
		{
			VertexComponent aux;
			aux.pos.x = verticesM[vertexIndices[i] - 1].x * escala;
			aux.pos.y = verticesM[vertexIndices[i] - 1].y * escala;
			aux.pos.z = verticesM[vertexIndices[i] - 1].z * escala;
			aux.normal.x = normales[normalIndices[i] - 1].x;
			aux.normal.y = normales[normalIndices[i] - 1].y;
			aux.normal.z = normales[normalIndices[i] - 1].z;
			aux.UV.x = uvs[uvIndices[i] - 1].u;
			aux.UV.y = 1 - uvs[uvIndices[i] - 1].v; //por que esta volteada la textura? no lo se, por eso le reste de 1
			//para enderezarla

			tempoVertices.push_back(aux);
		}

		cantvert = tempoVertices.size();
		cantind = tempoVertices.size();

		vertices = new VertexComponent[tempoVertices.size()];
		indices = new UINT[tempoVertices.size()];

		for (int i = 0; i < tempoVertices.size(); i++)
		{
			vertices[i] = tempoVertices[i];

			indices[i] = i;
		}

		for (int i = 0; i < tempoVertices.size(); i += 3)
		{
			D3DXVECTOR3 edge1;
			edge1 = vertices[indices[i] + 2].pos - vertices[indices[i]].pos;

			D3DXVECTOR3 edge2;
			edge2 = vertices[indices[i] + 1].pos - vertices[indices[i]].pos;


			float d1 = sqrt(vertices[indices[i] + 2].UV.x * vertices[indices[i] + 2].UV.x
				+ vertices[indices[i] + 2].UV.y * vertices[indices[i] + 2].UV.y);
			float d2 = sqrt(vertices[indices[i]].UV.x * vertices[indices[i]].UV.x
				+ vertices[indices[i]].UV.y * vertices[indices[i]].UV.y);
			float d3 = sqrt(vertices[indices[i] + 1].UV.x * vertices[indices[i] + 1].UV.x
				+ vertices[indices[i] + 1].UV.y * vertices[indices[i] + 1].UV.y);

			vector2 deltaUV1;
			float ddu1, ddu2;
			float ddv1, ddv2;
			float ddu3, ddv3;

			if (abs(d1) > 0.0001)
				ddu1 = vertices[indices[i] + 2].UV.x / d1;
			else
				ddu1 = 1000;

			if (abs(d2) > 0.0001)
				ddu2 = vertices[indices[i]].UV.x / d2;
			else
				ddu2 = 1000;

			if (abs(d1) > 0.0001)
				ddv1 = vertices[indices[i] + 2].UV.y / d1;
			else
				ddv1 = 1000;

			if (abs(d2) > 0.0001)
				ddv2 = vertices[indices[i]].UV.y / d2;
			else
				ddv2 = 1000;

			if (abs(d3) > 0.0001)
				ddu3 = vertices[indices[i] + 1].UV.x / d3;
			else
				ddu3 = 1000;

			if (abs(d3) > 0.0001)
				ddv3 = vertices[indices[i] + 1].UV.y / d3;
			else
				ddv3 = 1000;

			deltaUV1.u = ddu1 - ddu2;
			deltaUV1.v = ddv1 - ddv2;
			vector2 deltaUV2;
			deltaUV2.u = ddu3 - ddu2;
			deltaUV2.v = ddv3 - ddv2;


			float denom = (deltaUV1.u * deltaUV2.v - deltaUV2.u * deltaUV1.v);
			if (abs(denom) < 0.0001)
				denom = 0.001;
			float f = 1.0f / denom;

			D3DXVECTOR3 tangent;
			tangent.x = f * (deltaUV2.v * edge1.x - deltaUV1.v * edge2.x);
			tangent.y = f * (deltaUV2.v * edge1.y - deltaUV1.v * edge2.y);
			tangent.z = f * (deltaUV2.v * edge1.z - deltaUV1.v * edge2.z);
			D3DXVec3Normalize(&tangent, &tangent);


			vertices[indices[i]].tangente = tangent;

			vertices[indices[i] + 1].tangente = tangent;

			vertices[indices[i] + 2].tangente = tangent;

			if (i > 3068)
			{
				int p = 0;
			}

		}

		int u = 0;
	}
};
#endif