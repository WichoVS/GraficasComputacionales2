#ifndef _dxrr
#define _dxrr
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <d3dx10math.h>
#include "TerrenoRR.h"
#include "Camara.h"
#include "SkyDome.h"
#include "ModeloRR.h"
#include "XACT3Util.h"
#include "FuncionesGC.h"
#include <algorithm>
#include "Billboard.h"


#define PI 3.14159265
class DXRR {

private:
	int ancho;
	int alto;

public:

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

	HINSTANCE hInstance;
	HWND hWnd;

	D3D_DRIVER_TYPE driverType;
	D3D_FEATURE_LEVEL featureLevel;

	ID3D11Device* d3dDevice;
	ID3D11DeviceContext* d3dContext;
	IDXGISwapChain* swapChain;
	ID3D11RenderTargetView* backBufferTarget;

	ID3D11Texture2D* depthTexture;
	ID3D11DepthStencilView* depthStencilView;

	ID3D11DepthStencilState* depthStencilState;
	ID3D11DepthStencilState* depthStencilDisabledState;

	ID3D11BlendState* alphaBlendState, * commonBlendState;

	int frameBillboard;
	int Contador;
	int ContFramesTarde;
	int ContFramesNoche;
	int ContFramesMañana;
	int SkinSky;
	int contadorGLSL;
	float DifAmb[2];
	float luzAmb[4];
	float posY, posX, posZ;
	float PosicionesLuzDinamic[3];

	bool col_CP, col_CG, col_Al, col_bordaCG, col_bordaCP, col_fogataCP, col_fogataCG, col_Piedra1, col_Piedra2, col_Piedra3, col_Piedra4, col_Piedra5;

	TerrenoRR* terreno;
	SkyDome* skydome;
	BillboardRR* billboard1_T_Top, * billboard2_T_Top, * billboard2_T_Left, * billboard2_T_Right, * billboard2_G,
		* billboard1_T_Left, * billboard1_T_Right, * billboard1_G, * billboard9;
	Camara* camara;
	ModeloRR* model, * borda1, * borda2, * a_Paredes, * a_Madera, * a_Piso, * a_Techo
		, * cG_Pared, * cG_Piso, * cG_PalosMadera, * cG_Techo, * cG_MaderaFront, * cG_Ventana, * cg_Marcos,
		* cP_Chimenea, * cP_Concreto, * cP_MaderaVan, * cP_MarcosVen, * cP_ParedMad, * cP_PisoExt, * cP_PisoInt
		, * cP_Puerta, * cP_Techo, * cP_Ventanas, * agua, * f_Madera, * f_Piedras, * f_Madera2, * f_Piedras2
		, * piedra1, * piedra2, * piedra3, * piedra4, * piedra5;

	Luces lucesToShader;
	Factores factoresToShader;
	LuzDire luzDirToShader;

	D3DXVECTOR4 luzAmbiental;
	D3DXVECTOR4 luzDifusa;
	D3DXVECTOR4 luzEspecular;
	float fad;
	float fa;
	float fas;
	D3DXVECTOR3 dirluz;

	float izqder;
	float arriaba;
	float vel;
	float angle;
	float luzx;
	float luzy;
	float luzz;
	bool breakpoint;
	bool colision;

	XACTINDEX cueIndex;
	CXACT3Util m_XACT3;



	DXRR(HWND hWnd, int Ancho, int Alto)
	{
		col_CP = false;
		col_CG = false;
		col_Al = false; 
		col_bordaCG = false;
		col_bordaCP = false; 
		col_fogataCP = false;
		col_fogataCG = false; 
		col_Piedra1 = false; 
		col_Piedra2 = false; 
		col_Piedra3 = false;
		col_Piedra4 = false;
		col_Piedra5 = false;
		ContFramesMañana = 80;
		ContFramesNoche = 40;
		ContFramesTarde = 80;
		contadorGLSL = 0;
		luzAmbiental = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
		luzDifusa = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
		luzEspecular = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
		fad = 0.4f;
		fa = 0.8f;
		fas = 3.0f;
		dirluz = D3DXVECTOR3(0.0f, 100.0f, 0.0f);
		angle = 0.0f;
		bool colision = false;
		float luzx = 0.0f;
		float luzy = 0.0f;
		float luzz = 0.0f;
		breakpoint = false;
		frameBillboard = 0;
		ancho = Ancho;
		alto = Alto;
		driverType = D3D_DRIVER_TYPE_NULL;
		featureLevel = D3D_FEATURE_LEVEL_11_0;
		d3dDevice = 0;
		d3dContext = 0;
		swapChain = 0;
		backBufferTarget = 0;
		IniciaD3D(hWnd);
		izqder = 0;
		arriaba = 0;
		//billCargaFuego();
		camara = new Camara(D3DXVECTOR3(0, 80, 6), D3DXVECTOR3(0, 80, 0), D3DXVECTOR3(0, 1, 0), Ancho, Alto);
		terreno = new TerrenoRR(L"T1_color.jpg", L"T1_specular.jpg", L"T1_normal.jpg",
			L"T2_color.jpg", L"T2_specular.jpg", L"T2_normal.jpg",
			L"T3_color.jpg", L"T3_specular.jpg", L"T3_normal.jpg",
			L"HeightMapFinal.png", 300, 300, d3dDevice, d3dContext);
		skydome = new SkyDome(32, 32, 100.0f, &d3dDevice, &d3dContext, L"SkyDome.png", L"SkyDome_Cloud.jpg", L"SkyDome_Night.jpg");
		//Billboards lado casa grande
		billboard2_T_Top = new BillboardRR(L"Assets/Billboards/Arboles/Arbolito2.png", L"Assets/Billboards/Arboles/Arbolito2_Normal.png",
			d3dDevice, d3dContext, 8, 110, 145, -130, 130);
		billboard2_T_Left = new BillboardRR(L"Assets/Billboards/Arboles/Arbolito3.png", L"Assets/Billboards/Arboles/Arbolito3_Normal.png",
			d3dDevice, d3dContext, 8, 40, 145, 130, 150);
		billboard2_T_Right = new BillboardRR(L"Assets/Billboards/Arboles/Arbolito4.png", L"Assets/Billboards/Arboles/Arbolito4_Normal.png",
			d3dDevice, d3dContext, 8, -10, 145, -150, -130);
		billboard2_G = new BillboardRR(L"Assets/Billboards/Arboles/pasto.png", L"Assets/Billboards/Arboles/pasto_Normal.png",
			d3dDevice, d3dContext, 1, 40, 110, -130, 130);


		//billboards lado casa pequeña
		billboard1_T_Top = new BillboardRR(L"Assets/Billboards/Arboles/Arbolito.png", L"Assets/Billboards/Arboles/Arbolito1_Normal.png",
			d3dDevice, d3dContext, 8, -145, -110, -130, 130);
		billboard1_T_Left = new BillboardRR(L"Assets/Billboards/Arboles/Arbolito3.png", L"Assets/Billboards/Arboles/Arbolito3_Normal.png",
			d3dDevice, d3dContext, 8, -145, -80, -150, -130);
		billboard1_T_Right = new BillboardRR(L"Assets/Billboards/Arboles/Arbolito4.png", L"Assets/Billboards/Arboles/Arbolito4_Normal.png",
			d3dDevice, d3dContext, 8, -145, -50, 130, 150);
		billboard1_G = new BillboardRR(L"Assets/Billboards/Arboles/pasto.png", L"Assets/Billboards/Arboles/pasto_Normal.png",
			d3dDevice, d3dContext, 1, -110, -80, -130, 130);


		borda1 = new ModeloRR("Assets/Borda1/Borda1.obj", L"Assets/Borda1/B1_Color.png", L"Assets/Borda1/B1_Normal.png"
			, L"Assets/Borda1/B1_Roughness.png", d3dDevice, d3dContext, 6.0f, 11.0f, 6.0f, 1.0f);
		borda2 = new ModeloRR("Assets/Borda2/Borda2.obj", L"Assets/Borda1/B1_Color.png", L"Assets/Borda1/B1_Normal.png"
			, L"Assets/Borda1/B1_Roughness.png", d3dDevice, d3dContext, -40.0f, 11.0f, -40.0f, 1.0f);
		////Almacen
		a_Paredes = new ModeloRR("Assets/Almacen/Paredes.obj", L"Assets/Almacen/Paredes/P_Color.png",
			L"Assets/Almacen/Paredes/P_Normal.png", L"Assets/Almacen/Paredes/P_Roughness.png",
			d3dDevice, d3dContext, 100.0f, 17.5f, 80.0f, 3.0f);
		a_Madera = new ModeloRR("Assets/Almacen/Madera.obj", L"Assets/Almacen/Madera/M_Color.png",
			L"Assets/Almacen/Madera/M_Normal.png", L"Assets/Almacen/Madera/M_Roughness.png",
			d3dDevice, d3dContext, 100.0f, 17.5f, 80.0f, 3.0f);
		a_Piso = new ModeloRR("Assets/Almacen/Piso.obj", L"Assets/Almacen/Piso/P_Color.png",
			L"Assets/Almacen/Piso/P_Normal.png", L"Assets/Almacen/Piso/P_Roughness.png",
			d3dDevice, d3dContext, 100.0f, 17.5f, 80.0f, 3.0f);
		a_Techo = new ModeloRR("Assets/Almacen/Techo.obj", L"Assets/Almacen/Techo/T_Color.jpg",
			L"Assets/Almacen/Techo/T_Normal.jpg", L"Assets/Almacen/Techo/T_Roughness.jpg",
			d3dDevice, d3dContext, 100.0f, 17.5f, 80.0f, 3.0f);

		////Casa Grande maxX 100.0f minX 10.0f maxY 30.0f  minY -30.0f
		cG_MaderaFront = new ModeloRR("Assets/CasaGrande/FrontMadera.obj", L"Assets/CasaGrande/MaderaFront/MF_Color.png",
			L"Assets/CasaGrande/MaderaFront/MF_Normal.png", L"Assets/CasaGrande/MaderaFront/MF_Roughness.png",
			d3dDevice, d3dContext, 100.0f, 17.5f, -30.0f, 3.0f);
		cg_Marcos = new ModeloRR("Assets/CasaGrande/MaderaMarcos.obj", L"Assets/CasaGrande/MaderaMarcos/MM_Color.png",
			L"Assets/CasaGrande/MaderaMarcos/MM_Normal.png", L"Assets/CasaGrande/MaderaMarcos/MM_Roughness.png",
			d3dDevice, d3dContext, 100.0f, 17.5f, -30.0f, 3.0f);
		cG_PalosMadera = new ModeloRR("Assets/CasaGrande/PalosMadera.obj", L"Assets/Almacen/Madera/M_Color.png",
			L"Assets/Almacen/Madera/M_Normal.png", L"Assets/Almacen/Madera/M_Roughness.png",
			d3dDevice, d3dContext, 100.0f, 17.5f, -30.0f, 3.0f);
		cG_Pared = new ModeloRR("Assets/CasaGrande/Paredes.obj", L"Assets/CasaGrande/MaderaParedes/MP_Color.png",
			L"Assets/CasaGrande/MaderaParedes/MP_Normal.png", L"Assets/CasaGrande/MaderaParedes/MP_Roughness.png",
			d3dDevice, d3dContext, 100.0f, 17.5f, -30.0f, 3.0f);
		cG_Piso = new ModeloRR("Assets/CasaGrande/Piso.obj", L"Assets/Almacen/Piso/P_Color.png",
			L"Assets/Almacen/Piso/P_Normal.png", L"Assets/Almacen/Piso/P_Roughness.png",
			d3dDevice, d3dContext, 100.0f, 17.5f, -30.0f, 3.0f);
		cG_Techo = new ModeloRR("Assets/CasaGrande/Techo.obj", L"Assets/Almacen/Techo/T_Color.jpg",
			L"Assets/Almacen/Techo/T_Normal.jpg", L"Assets/Almacen/Techo/T_Roughness.jpg",
			d3dDevice, d3dContext, 100.0f, 17.5f, -30.0f, 3.0f);
		cG_Ventana = new ModeloRR("Assets/CasaGrande/Ventana.obj", L"Assets/CasaGrande/Ventanas/Glass.jpg",
			L"Assets/CasaGrande/Ventanas/G_Normal.png", L"Assets/CasaGrande/Ventanas/G_Roughness.png",
			d3dDevice, d3dContext, 100.0f, 17.5f, -30.0f, 3.0f);
		
		
		//ColisionCasaPequeña maxX -75 minX -125 maxY 40.0f  minY -80
		////Casa Pequeña cP_Chimenea, cP_Concreto, cP_MaderaVan, cP_MarcosVen, cP_ParedMad, cP_PisoExt, cP_PisoInt, cP_Puerta, cP_Techo, cP_Ventanas;
		cP_Chimenea = new ModeloRR("Assets/CasaPequeña/CP_Chimenea.obj", L"Assets/CasaPequeña/Chimenea/Ch_Color.png",
			L"Assets/CasaPequeña/Chimenea/Ch_Normal.png", L"Assets/CasaPequeña/Chimenea/Ch_Roughness.png",
			d3dDevice, d3dContext, -100.0f, 17.5f, -60.0f, 3.0f);
		cP_Concreto = new ModeloRR("Assets/CasaPequeña/CP_Concreto.obj", L"Assets/CasaPequeña/Concreto/C_Color.png",
			L"Assets/CasaPequeña/Concreto/C_Normal.png", L"Assets/CasaPequeña/Concreto/C_Roughness.png",
			d3dDevice, d3dContext, -100.0f, 17.5f, -60.0f, 3.0f);
		cP_MaderaVan = new ModeloRR("Assets/CasaPequeña/CP_MaderaVan.obj", L"Assets/CasaPequeña/MaderaVanish/MV_Color.png",
			L"Assets/CasaPequeña/MaderaVanish/MV_Normal.png", L"Assets/CasaPequeña/MaderaVanish/MV_Roughness.png",
			d3dDevice, d3dContext, -100.0f, 17.5f, -60.0f, 3.0f);
		cP_MarcosVen = new ModeloRR("Assets/CasaPequeña/CP_MarcosMadera.obj", L"Assets/CasaPequeña/MaderaMarcos/MM_Color.png",
			L"Assets/CasaPequeña/MaderaMarcos/MM_Normal.png", L"Assets/CasaPequeña/MaderaMarcos/MM_Roughness.png",
			d3dDevice, d3dContext, -100.0f, 17.5f, -60.0f, 3.0f);
		cP_ParedMad = new ModeloRR("Assets/CasaPequeña/CP_ParedesMadera.obj", L"Assets/Almacen/Paredes/P_Color.png",
			L"Assets/Almacen/Paredes/P_Normal.png", L"Assets/Almacen/Paredes/P_Roughness.png",
			d3dDevice, d3dContext, -100.0f, 17.5f, -60.0f, 3.0f);
		cP_PisoExt = new ModeloRR("Assets/CasaPequeña/CP_PisoExt.obj", L"Assets/Almacen/Paredes/P_Color.png",
			L"Assets/Almacen/Paredes/P_Normal.png", L"Assets/Almacen/Paredes/P_Roughness.png",
			d3dDevice, d3dContext, -100.0f, 17.5f, -60.0f, 3.0f);
		cP_PisoInt = new ModeloRR("Assets/CasaPequeña/CP_PisoInt.obj", L"Assets/CasaPequeña/Piso/Piso_Color.jpeg",
			L"Assets/CasaPequeña/Piso/Piso_Normal.jpeg", L"Assets/CasaPequeña/Piso/Piso_Roughness.jpeg",
			d3dDevice, d3dContext, -100.0f, 17.5f, -60.0f, 3.0f);
		cP_Puerta = new ModeloRR("Assets/CasaPequeña/CP_Puerta.obj", L"Assets/CasaGrande/MaderaFront/MF_Color.png",
			L"Assets/CasaGrande/MaderaFront/MF_Normal.png", L"Assets/CasaGrande/MaderaFront/MF_Roughness.png",
			d3dDevice, d3dContext, -100.0f, 17.5f, -60.0f, 3.0f);
		cP_Techo = new ModeloRR("Assets/CasaPequeña/CP_Techo.obj", L"Assets/Almacen/Techo/T_Color.jpg",
			L"Assets/Almacen/Techo/T_Normal.jpg", L"Assets/Almacen/Techo/T_Roughness.jpg",
			d3dDevice, d3dContext, -100.0f, 17.5f, -60.0f, 3.0f);
		cP_Ventanas = new ModeloRR("Assets/CasaPequeña/CP_Ventanas.obj", L"Assets/CasaGrande/Ventanas/Glass.jpg",
			L"Assets/CasaGrande/Ventanas/G_Normal.png", L"Assets/CasaGrande/Ventanas/G_Roughness.png",
			d3dDevice, d3dContext, -100.0f, 17.5f, -60.0f, 3.0f);

		////Agua
		agua = new ModeloRR("Assets/Agua/Agua.obj", L"Assets/Agua/Agua_Color.jpg",
			L"Assets/Agua/Agua_Normal.jpg", L"Assets/Agua/Agua_Specular.jpg",
			d3dDevice, d3dContext, -30.0f, 11.0f, 0.0f, 2.0f);

		////Fogata
		f_Madera = new ModeloRR("Assets/Fogata/Fogata_Madera.obj", L"Assets/Fogata/Tronco_Color.png",
			L"Assets/Fogata/Tronco_Normal.png", L"Assets/Fogata/Tronco_Roughness.png",
			d3dDevice, d3dContext, 60.0f, 15.0f, -80.0f, 3.0f);
		f_Piedras = new ModeloRR("Assets/Fogata/Fogata_Piedra.obj", L"Assets/Fogata/RocaFogata_Color.png",
			L"Assets/Fogata/RocaFogata_Normal.png", L"Assets/Fogata/RocaFogata_Roughness.png",
			d3dDevice, d3dContext, 60.0f, 15.0f, -80.0f, 3.0f);
		////FogataLadoCasaPequeña
		f_Madera2 = new ModeloRR("Assets/Fogata/Fogata_Madera.obj", L"Assets/Fogata/Tronco_Color.png",
			L"Assets/Fogata/Tronco_Normal.png", L"Assets/Fogata/Tronco_Roughness.png",
			d3dDevice, d3dContext, -80.0f, 18.0f, 80.0f, 3.0f);
		f_Piedras2 = new ModeloRR("Assets/Fogata/Fogata_Piedra.obj", L"Assets/Fogata/RocaFogata_Color.png",
			L"Assets/Fogata/RocaFogata_Normal.png", L"Assets/Fogata/RocaFogata_Roughness.png",
			d3dDevice, d3dContext, -80.0f, 18.0f, 80.0f, 3.0f);


		////Piedras
		piedra1 = new ModeloRR("Assets/Piedra/Piedra1.obj", L"Assets/Piedra/P_Color.jpeg",
			L"Assets/Piedra/P_Normal.jpeg", L"Assets/Piedra/P_Roughness.png",
			d3dDevice, d3dContext, -100.0f, 16.0f, 50.0f, 1.0f);
		piedra2 = new ModeloRR("Assets/Piedra/Piedra2.obj", L"Assets/Piedra/P_Color.jpeg",
			L"Assets/Piedra/P_Normal.jpeg", L"Assets/Piedra/P_Roughness.png",
			d3dDevice, d3dContext, -80.0f, 19.0f, 120.0f, 1.0f);
		piedra3 = new ModeloRR("Assets/Piedra/Piedra3.obj", L"Assets/Piedra/P_Color.jpeg",
			L"Assets/Piedra/P_Normal.jpeg", L"Assets/Piedra/P_Roughness.png",
			d3dDevice, d3dContext, -70.0f, 16.0f, 60.0f, 1.0f);
		piedra4 = new ModeloRR("Assets/Piedra/Piedra4.obj", L"Assets/Piedra/P_Color.jpeg",
			L"Assets/Piedra/P_Normal.jpeg", L"Assets/Piedra/P_Roughness.png",
			d3dDevice, d3dContext, -80.0f, 16.0f, 30.0f, 1.0f);
		piedra5 = new ModeloRR("Assets/Piedra/Piedra5.obj", L"Assets/Piedra/P_Color.jpeg",
			L"Assets/Piedra/P_Normal.jpeg", L"Assets/Piedra/P_Roughness.png",
			d3dDevice, d3dContext, -90.0f, 19.0f, 100.0f, 1.0f);


	}

	~DXRR()
	{
		LiberaD3D();
		m_XACT3.Terminate();
	}

	bool IniciaD3D(HWND hWnd)
	{
		this->hInstance = hInstance;
		this->hWnd = hWnd;

		//obtiene el ancho y alto de la ventana donde se dibuja
		RECT dimensions;
		GetClientRect(hWnd, &dimensions);
		unsigned int width = dimensions.right - dimensions.left;
		unsigned int heigth = dimensions.bottom - dimensions.top;

		//Las formas en como la pc puede ejecutar el DX11, la mas rapida es D3D_DRIVER_TYPE_HARDWARE pero solo se puede usar cuando lo soporte el hardware
		//otra opcion es D3D_DRIVER_TYPE_WARP que emula el DX11 en los equipos que no lo soportan
		//la opcion menos recomendada es D3D_DRIVER_TYPE_SOFTWARE, es la mas lenta y solo es util cuando se libera una version de DX que no sea soportada por hardware
		D3D_DRIVER_TYPE driverTypes[] =
		{
			D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP, D3D_DRIVER_TYPE_SOFTWARE
		};
		unsigned int totalDriverTypes = ARRAYSIZE(driverTypes);

		//La version de DX que utilizara, en este caso el DX11
		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0
		};
		unsigned int totalFeaturesLevels = ARRAYSIZE(featureLevels);

		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
		swapChainDesc.BufferCount = 1;
		swapChainDesc.BufferDesc.Width = width;
		swapChainDesc.BufferDesc.Height = heigth;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = hWnd;
		swapChainDesc.Windowed = true;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;

		HRESULT result;
		unsigned int driver = 0, creationFlags = 0;
		for (driver = 0; driver < totalDriverTypes; driver++)
		{
			result = D3D11CreateDeviceAndSwapChain(0, driverTypes[driver], 0,
				creationFlags, featureLevels, totalFeaturesLevels,
				D3D11_SDK_VERSION, &swapChainDesc, &swapChain,
				&d3dDevice, &featureLevel, &d3dContext);

			if (SUCCEEDED(result))
			{
				driverType = driverTypes[driver];
				break;
			}
		}

		if (FAILED(result))
		{

			//Error al crear el Direct3D device
			return false;
		}

		ID3D11Texture2D* backBufferTexture;
		result = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferTexture);
		if (FAILED(result))
		{
			//"Error al crear el swapChainBuffer
			return false;
		}

		result = d3dDevice->CreateRenderTargetView(backBufferTexture, 0, &backBufferTarget);
		if (backBufferTexture)
			backBufferTexture->Release();

		if (FAILED(result))
		{
			//Error al crear el renderTargetView
			return false;
		}


		D3D11_VIEWPORT viewport;
		viewport.Width = (FLOAT)width;
		viewport.Height = (FLOAT)heigth;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;

		d3dContext->RSSetViewports(1, &viewport);

		D3D11_TEXTURE2D_DESC depthTexDesc;
		ZeroMemory(&depthTexDesc, sizeof(depthTexDesc));
		depthTexDesc.Width = width;
		depthTexDesc.Height = heigth;
		depthTexDesc.MipLevels = 1;
		depthTexDesc.ArraySize = 1;
		depthTexDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthTexDesc.SampleDesc.Count = 1;
		depthTexDesc.SampleDesc.Quality = 0;
		depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
		depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthTexDesc.CPUAccessFlags = 0;
		depthTexDesc.MiscFlags = 0;

		result = d3dDevice->CreateTexture2D(&depthTexDesc, NULL, &depthTexture);
		if (FAILED(result))
		{
			MessageBox(0, L"Error", L"Error al crear la DepthTexture", MB_OK);
			return false;
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
		ZeroMemory(&descDSV, sizeof(descDSV));
		descDSV.Format = depthTexDesc.Format;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;

		result = d3dDevice->CreateDepthStencilView(depthTexture, &descDSV, &depthStencilView);
		if (FAILED(result))
		{
			MessageBox(0, L"Error", L"Error al crear el depth stencil target view", MB_OK);
			return false;
		}

		d3dContext->OMSetRenderTargets(1, &backBufferTarget, depthStencilView);

		return true;

	}

	void LiberaD3D(void)
	{
		if (depthTexture)
			depthTexture->Release();
		if (depthStencilView)
			depthStencilView->Release();
		if (backBufferTarget)
			backBufferTarget->Release();
		if (swapChain)
			swapChain->Release();
		if (d3dContext)
			d3dContext->Release();
		if (d3dDevice)
			d3dDevice->Release();

		depthTexture = 0;
		depthStencilView = 0;
		d3dDevice = 0;
		d3dContext = 0;
		swapChain = 0;
		backBufferTarget = 0;
	}

	void Render(void)
	{
		float sphere[3] = { 0,0,0 };
		float prevPos[3] = { camara->posCam.x, camara->posCam.y, camara->posCam.z };
		Contador++;
		


		float clearColor[4] = { 0, 0, 0, 1.0f };
		d3dContext->ClearRenderTargetView(backBufferTarget, clearColor);
		d3dContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

		D3DXVECTOR3 campos = D3DXVECTOR3(camara->posCam.x, camara->posCam.y, camara->posCam.z);

		if (Contador % 20 == 0)
		{

			if (ContFramesMañana != 0 && ContFramesTarde != 0 && ContFramesNoche != 0)
			{
				DifAmb[0] = 0.8f;
				DifAmb[1] = 0.6f;
				ContFramesMañana--;
				SkinSky = 0;
				contadorGLSL++;
			}

			if (ContFramesTarde != 0 && ContFramesMañana == 0 && ContFramesNoche != 0)
			{
				DifAmb[0] = 0.8f;
				DifAmb[1] = 0.9f;
				ContFramesTarde--;
				SkinSky = 1*10;
				contadorGLSL++;
			}

			if (ContFramesMañana == 0 && ContFramesTarde == 0 && ContFramesNoche != 0)
			{
				DifAmb[0] = 0.2f;
				DifAmb[1] = 0.1f;
				ContFramesNoche--;
				SkinSky = 2*10;
				contadorGLSL++;
			}

			if (ContFramesNoche == 0)
			{
				ContFramesMañana = 80;
				ContFramesNoche = 40;
				ContFramesTarde = 80;
				contadorGLSL = 0;
			}
		}


		fa = DifAmb[1];
		fad = DifAmb[0];

		if (angle < 180.0f)
		{
			if (angle > 40.0f && angle < 140.0f)
				angle += 0.005f;
			else
			{
				angle += 0.010f;
			}
		}

		if (angle >= 180.0f)
			angle += 0.030f;


		if (angle >= 360.0f)
			angle = 0.0f;

		colision = false;
		if (d3dContext == 0)
			return;



		luzy = sin((angle * 41.2723) * PI / 180) * 350;
		luzx = 0;// cos((rotation * 41.2723) * PI / 180) * 350;
		luzz = cos((angle * 41.2723) * PI / 180) * 350;

		dirluz.x = luzx;
		dirluz.y = luzy;
		dirluz.z = luzz;


		camara->posCam.y = terreno->Superficie(camara->posCam.x, camara->posCam.z) + 5;
		skydome->Update(camara->vista, camara->proyeccion);

		float camPosXZ[2] = { camara->posCam.x, camara->posCam.z };

		TurnOffDepth();
		skydome->Render(camara->posCam, SkinSky, contadorGLSL);
		TurnOnDepth();
		terreno->Draw(camara->vista, camara->proyeccion, luzAmbiental, luzDifusa, luzEspecular
			, fad, fa, fas, dirluz);
		//TurnOnAlphaBlending(); b1 -110, 0 || b2  -145, 0  ///// b1 -110, -80 || b2 -145, 80

		//Billboard lado de la casa Grande
		for (int x = 0; x < 120; x++) {

			billboard2_T_Top->Draw(camara->vista, camara->proyeccion, camara->posCam,
				terreno->Superficie(billboard2_T_Top->posX[x], billboard2_T_Top->posZ[x]), billboard2_T_Top->posX[x], billboard2_T_Top->posZ[x],
				luzAmbiental, luzDifusa, luzEspecular, fad, fa, fas, dirluz);
		}
		for (int x = 0; x < 120; x++) {

			billboard2_T_Left->Draw(camara->vista, camara->proyeccion, camara->posCam,
				terreno->Superficie(billboard2_T_Left->posX[x], billboard2_T_Left->posZ[x]), billboard2_T_Left->posX[x], billboard2_T_Left->posZ[x],
				luzAmbiental, luzDifusa, luzEspecular, fad, fa, fas, dirluz);
		}
		for (int x = 0; x < 120; x++) {

			billboard2_T_Right->Draw(camara->vista, camara->proyeccion, camara->posCam,
				terreno->Superficie(billboard2_T_Right->posX[x], billboard2_T_Right->posZ[x]), billboard2_T_Right->posX[x], billboard2_T_Right->posZ[x],
				luzAmbiental, luzDifusa, luzEspecular, fad, fa, fas, dirluz);
		}		for (int x = 0; x < 120; x++) {

			billboard2_G->Draw(camara->vista, camara->proyeccion, camara->posCam,
				terreno->Superficie(billboard2_G->posX[x], billboard2_G->posZ[x]), billboard2_G->posX[x], billboard2_G->posZ[x],
				luzAmbiental, luzDifusa, luzEspecular, fad, fa, fas, dirluz);
		}

		//Billboard lado de la casa pequeña
		for (int x = 0; x < 120; x++) {

			billboard1_T_Top->Draw(camara->vista, camara->proyeccion, camara->posCam,
				terreno->Superficie(billboard1_T_Top->posX[x], billboard1_T_Top->posZ[x]), billboard1_T_Top->posX[x], billboard1_T_Top->posZ[x],
				luzAmbiental, luzDifusa, luzEspecular, fad, fa, fas, dirluz);
		}
		for (int x = 0; x < 120; x++) {

			billboard1_T_Left->Draw(camara->vista, camara->proyeccion, camara->posCam,
				terreno->Superficie(billboard1_T_Left->posX[x], billboard1_T_Left->posZ[x]), billboard1_T_Left->posX[x], billboard1_T_Left->posZ[x],
				luzAmbiental, luzDifusa, luzEspecular, fad, fa, fas, dirluz);
		}
		for (int x = 0; x < 120; x++) {

			billboard1_T_Right->Draw(camara->vista, camara->proyeccion, camara->posCam,
				terreno->Superficie(billboard1_T_Right->posX[x], billboard1_T_Right->posZ[x]), billboard1_T_Right->posX[x], billboard1_T_Right->posZ[x],
				luzAmbiental, luzDifusa, luzEspecular, fad, fa, fas, dirluz);
		}
		for (int x = 0; x < 120; x++) {

			billboard1_G->Draw(camara->vista, camara->proyeccion, camara->posCam,
				terreno->Superficie(billboard1_G->posX[x], billboard1_G->posZ[x]), billboard1_G->posX[x], billboard1_G->posZ[x],
				luzAmbiental, luzDifusa, luzEspecular, fad, fa, fas, dirluz);
		}

		//TurnOffAlphaBlending();
		D3DXVECTOR4 camaraxd;
		camaraxd.x = camara->posCam.x;
		camaraxd.y = camara->posCam.y;
		camaraxd.z = camara->posCam.z;
		camaraxd.w = 0.0f;

		//model->Draw(camara->vista, camara->proyeccion, 0.0f, camaraxd);
		borda1->Draw(camara->vista, camara->proyeccion, 0.0f, camaraxd, luzAmbiental, luzDifusa, luzEspecular
			, fad, fa, fas, dirluz);
		borda2->Draw(camara->vista, camara->proyeccion, 0.0f, camaraxd, luzAmbiental, luzDifusa, luzEspecular
			, fad, fa, fas, dirluz);
		//*a_Paredes, * a_Madera, * a_Piso, * a_Techo
		a_Paredes->Draw(camara->vista, camara->proyeccion, 3, camaraxd, luzAmbiental, luzDifusa, luzEspecular
			, fad, fa, fas, dirluz);
		a_Madera->Draw(camara->vista, camara->proyeccion, 3, camaraxd, luzAmbiental, luzDifusa, luzEspecular
			, fad, fa, fas, dirluz);
		a_Piso->Draw(camara->vista, camara->proyeccion, 3, camaraxd, luzAmbiental, luzDifusa, luzEspecular
			, fad, fa, fas, dirluz);
		a_Techo->Draw(camara->vista, camara->proyeccion, 3, camaraxd, luzAmbiental, luzDifusa, luzEspecular
			, fad, fa, fas, dirluz);
		//*cG_Pared, * cG_Piso, * cG_PalosMadera, * cG_Techo, * cG_MaderaFront, * cG_Ventana, * cg_Marcos;
		cG_MaderaFront->Draw(camara->vista, camara->proyeccion, 3.0f, camaraxd, luzAmbiental, luzDifusa, luzEspecular
			, fad, fa, fas, dirluz);
		cg_Marcos->Draw(camara->vista, camara->proyeccion, 3.0f, camaraxd, luzAmbiental, luzDifusa, luzEspecular
			, fad, fa, fas, dirluz);
		cG_PalosMadera->Draw(camara->vista, camara->proyeccion, 3.0f, camaraxd, luzAmbiental, luzDifusa, luzEspecular
			, fad, fa, fas, dirluz);
		cG_Pared->Draw(camara->vista, camara->proyeccion, 3.0f, camaraxd, luzAmbiental, luzDifusa, luzEspecular
			, fad, fa, fas, dirluz);
		cG_Piso->Draw(camara->vista, camara->proyeccion, 3.0f, camaraxd, luzAmbiental, luzDifusa, luzEspecular
			, fad, fa, fas, dirluz);
		cG_Techo->Draw(camara->vista, camara->proyeccion, 3.0f, camaraxd, luzAmbiental, luzDifusa, luzEspecular
			, fad, fa, fas, dirluz);
		cG_Ventana->Draw(camara->vista, camara->proyeccion, 3.0f, camaraxd, luzAmbiental, luzDifusa, luzEspecular
			, fad, fa, fas, dirluz);

		////Casa Pequeña cP_Chimenea, cP_Concreto, cP_MaderaVan, cP_MarcosVen, cP_ParedMad, cP_PisoExt, cP_PisoInt, cP_Puerta, cP_Techo, cP_Ventanas;
		cP_Chimenea->Draw(camara->vista, camara->proyeccion, 0.0f, camaraxd, luzAmbiental, luzDifusa, luzEspecular
			, fad, fa, fas, dirluz);
		cP_Concreto->Draw(camara->vista, camara->proyeccion, 0.0f, camaraxd, luzAmbiental, luzDifusa, luzEspecular
			, fad, fa, fas, dirluz);
		cP_MaderaVan->Draw(camara->vista, camara->proyeccion, 0.0f, camaraxd, luzAmbiental, luzDifusa, luzEspecular
			, fad, fa, fas, dirluz);
		cP_MarcosVen->Draw(camara->vista, camara->proyeccion, 0.0f, camaraxd, luzAmbiental, luzDifusa, luzEspecular
			, fad, fa, fas, dirluz);
		cP_ParedMad->Draw(camara->vista, camara->proyeccion, 0.0f, camaraxd, luzAmbiental, luzDifusa, luzEspecular
			, fad, fa, fas, dirluz);
		cP_PisoExt->Draw(camara->vista, camara->proyeccion, 0.0f, camaraxd, luzAmbiental, luzDifusa, luzEspecular
			, fad, fa, fas, dirluz);
		cP_PisoInt->Draw(camara->vista, camara->proyeccion, 0.0f, camaraxd, luzAmbiental, luzDifusa, luzEspecular
			, fad, fa, fas, dirluz);
		cP_Puerta->Draw(camara->vista, camara->proyeccion, 0.0f, camaraxd, luzAmbiental, luzDifusa, luzEspecular
			, fad, fa, fas, dirluz);
		cP_Techo->Draw(camara->vista, camara->proyeccion, 0.0f, camaraxd, luzAmbiental, luzDifusa, luzEspecular
			, fad, fa, fas, dirluz);
		cP_Ventanas->Draw(camara->vista, camara->proyeccion, 0.0f, camaraxd, luzAmbiental, luzDifusa, luzEspecular
			, fad, fa, fas, dirluz);

		agua->Draw(camara->vista, camara->proyeccion, 0.0f, camaraxd, luzAmbiental, luzDifusa, luzEspecular
			, fad, fa, fas, dirluz);

		f_Madera->Draw(camara->vista, camara->proyeccion, 0.0f, camaraxd, luzAmbiental, luzDifusa, luzEspecular
			, fad, fa, fas, dirluz);
		f_Piedras->Draw(camara->vista, camara->proyeccion, 0.0f, camaraxd, luzAmbiental, luzDifusa, luzEspecular
			, fad, fa, fas, dirluz);
		f_Madera2->Draw(camara->vista, camara->proyeccion, 0.0f, camaraxd, luzAmbiental, luzDifusa, luzEspecular
			, fad, fa, fas, dirluz);
		f_Piedras2->Draw(camara->vista, camara->proyeccion, 0.0f, camaraxd, luzAmbiental, luzDifusa, luzEspecular
			, fad, fa, fas, dirluz);

		piedra1->Draw(camara->vista, camara->proyeccion, 0.0f, camaraxd, luzAmbiental, luzDifusa, luzEspecular
			, fad, fa, fas, dirluz);
		piedra2->Draw(camara->vista, camara->proyeccion, 0.0f, camaraxd, luzAmbiental, luzDifusa, luzEspecular
			, fad, fa, fas, dirluz);
		piedra3->Draw(camara->vista, camara->proyeccion, 0.0f, camaraxd, luzAmbiental, luzDifusa, luzEspecular
			, fad, fa, fas, dirluz);
		piedra4->Draw(camara->vista, camara->proyeccion, 0.0f, camaraxd, luzAmbiental, luzDifusa, luzEspecular
			, fad, fa, fas, dirluz);
		piedra5->Draw(camara->vista, camara->proyeccion, 0.0f, camaraxd, luzAmbiental, luzDifusa, luzEspecular
			, fad, fa, fas, dirluz);




		////Casa Grande maxX 100.0f minX 10.0f maxY 30.0f  minY -30.0f
		camara->UpdateCam(vel, arriaba, izqder);

		////Piedra1 -100.0f, 16.0f, 50.0f minX -102.0f maxX -98.0f minY 48.0f maxY 52.0f;
		col_Piedra1 = CheckCollision(camara->posCam.x, camara->posCam.z,-102.0f, -98.0f, 48.0f, 52.0f);
		if (col_Piedra1)
		{
			camara->posCam.x = prevPos[0];
			camara->posCam.z = prevPos[2];
		}

		////Piedra2 -80.0f, 19.0f, 120.0f minX  -82.0f maxX -78.0f minY 118.0f maxY 122.0f
		col_Piedra2 = CheckCollision(camara->posCam.x, camara->posCam.z, -82.0f, -78.0f, 118.0f, 122.0f);
		if (col_Piedra2)
		{
			camara->posCam.x = prevPos[0];
			camara->posCam.z = prevPos[2];
		}

		////Piedra3 -70.0f, 16.0f, 60.0f minX -72.0f maxX -68.0f minY 58.0f maxY 62.0f
		col_Piedra3 = CheckCollision(camara->posCam.x, camara->posCam.z, -72.0f, -68.0f, 58.0f, 62.0f);
		if (col_Piedra3)
		{
			camara->posCam.x = prevPos[0];
			camara->posCam.z = prevPos[2];
		}

		////Piedra4 -80.0f, 16.0f, 30.0f minX -82.0f maxX -78.0f minY 28.0f maxY 32.0f
		col_Piedra4 = CheckCollision(camara->posCam.x, camara->posCam.z, -82.0f, -78.0f, 28.0f, 32.0f);
		if (col_Piedra4)
		{
			camara->posCam.x = prevPos[0];
			camara->posCam.z = prevPos[2];
		}

		////Piedra5 -90.0f, 19.0f, 100.0f minX -92.0f maxX -88.0f minY 98.0f maxY 102.0f
		col_Piedra5 = CheckCollision(camara->posCam.x, camara->posCam.z, -92.0f, -88.0f, 98.0f, 102.0f);
		if (col_Piedra5)
		{
			camara->posCam.x = prevPos[0];
			camara->posCam.z = prevPos[2];
		}

		////FogataCasaPequeña -80.0f, 18.0f, 80.0f minX -82.0f maxX -78.0f minY 78.0f maxY 82.0f
		col_fogataCP = CheckCollision(camara->posCam.x, camara->posCam.z, -82.0f, -78.0f, 78.0f, 84.0f);
		if (col_fogataCP)
		{
			camara->posCam.x = prevPos[0];
			camara->posCam.z = prevPos[2];
		}

		////FogataCasaGrande 60.0f, 15.0f, -80.0f minX 58.0f maxX 62.0f minY -82f maxY -76.0f
		col_fogataCG = CheckCollision(camara->posCam.x, camara->posCam.z, 58.0f, 62.0f, -82.0f, -76.0f);
		if (col_fogataCG)
		{
			camara->posCam.x = prevPos[0];
			camara->posCam.z = prevPos[2];
		}

		////BordaCasaGrande 6.0f, 11.0f, 6.0f minX 2.0f maxX 10.0f minY 3.0f maxY 9.0f coment
		col_bordaCG = CheckCollision(camara->posCam.x, camara->posCam.z, 2.0f, 10.0f, 3.0f, 9.0f);
		if (col_bordaCG)
		{
			camara->posCam.x = prevPos[0];
			camara->posCam.z = prevPos[2];
		}
		////BordaCasaGrande -40.0f, 11.0f, -40.0f minX -47.0f maxX -33.0f minY -43.0f maxY -37.0f
		col_bordaCP = CheckCollision(camara->posCam.x, camara->posCam.z, -47.0f, -33.0f, -43.0f, -37.0f);
		if (col_bordaCP)
		{
			camara->posCam.x = prevPos[0];
			camara->posCam.z = prevPos[2];
		}

		////Almacen 100.0f, 17.5f, 80.0f minX 90.0f maxX100.0f minY 73.0f  maxY 80.0f
		col_Al = CheckCollision(camara->posCam.x, camara->posCam.z, 70.0f, 100.0f, 73.0f, 90.0f);
		if (col_Al)
		{
			camara->posCam.x = prevPos[0];
			camara->posCam.z = prevPos[2];
		}

		////CasaGrande 100.0f, 17.5f, -30.0f minX 15.0f maxX 120.0f minY -30.0f maxY 50.0f
		col_CG = CheckCollision(camara->posCam.x, camara->posCam.z, 15.0f, 120.0f, -50.0f, 9.0f);
		if (col_CG)
		{
			camara->posCam.x = prevPos[0];
			camara->posCam.z = prevPos[2];
		}

		////CasaPequeña -100.0f, 17.5f, -60.0f minX -110.0f  maxX -90.0f minY -75.0f maxY -45.0f
		col_CP = CheckCollision(camara->posCam.x, camara->posCam.z, -115.0f, -85.0f, -85.0f, -40.0f);
		if (col_CP)
		{
			camara->posCam.x = prevPos[0];
			camara->posCam.z = prevPos[2];
		}

		swapChain->Present(1, 0);
	}

	bool isPointInsideSphere(float* point, float* sphere) {
		bool collition = false;

		float distance = sqrt((point[0] - sphere[0]) * (point[0] - sphere[0]) +
			(point[1] - sphere[1]) * (point[1] - sphere[1]));

		if (distance < sphere[2])
			collition = true;
		return collition;
	}

	//Activa el alpha blend para dibujar con transparencias
	void TurnOnAlphaBlending()
	{
		float blendFactor[4];
		blendFactor[0] = 0.0f;
		blendFactor[1] = 0.0f;
		blendFactor[2] = 0.0f;
		blendFactor[3] = 0.0f;
		HRESULT result;

		D3D11_BLEND_DESC descABSD;
		ZeroMemory(&descABSD, sizeof(D3D11_BLEND_DESC));
		descABSD.RenderTarget[0].BlendEnable = TRUE;
		descABSD.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		descABSD.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		descABSD.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		descABSD.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		descABSD.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		descABSD.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		descABSD.RenderTarget[0].RenderTargetWriteMask = 0x0f;

		result = d3dDevice->CreateBlendState(&descABSD, &alphaBlendState);
		if (FAILED(result))
		{
			MessageBox(0, L"Error", L"Error al crear el blend state", MB_OK);
			return;
		}

		d3dContext->OMSetBlendState(alphaBlendState, blendFactor, 0xffffffff);
	}

	//Regresa al blend normal(solido)
	void TurnOffAlphaBlending()
	{
		D3D11_BLEND_DESC descCBSD;
		ZeroMemory(&descCBSD, sizeof(D3D11_BLEND_DESC));
		descCBSD.RenderTarget[0].BlendEnable = FALSE;
		descCBSD.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		descCBSD.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		descCBSD.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		descCBSD.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		descCBSD.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		descCBSD.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		descCBSD.RenderTarget[0].RenderTargetWriteMask = 0x0f;
		HRESULT result;

		result = d3dDevice->CreateBlendState(&descCBSD, &commonBlendState);
		if (FAILED(result))
		{
			MessageBox(0, L"Error", L"Error al crear el blend state", MB_OK);
			return;
		}

		d3dContext->OMSetBlendState(commonBlendState, NULL, 0xffffffff);
	}

	void TurnOnDepth()
	{
		D3D11_DEPTH_STENCIL_DESC descDSD;
		ZeroMemory(&descDSD, sizeof(descDSD));
		descDSD.DepthEnable = true;
		descDSD.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		descDSD.DepthFunc = D3D11_COMPARISON_LESS;
		descDSD.StencilEnable = true;
		descDSD.StencilReadMask = 0xFF;
		descDSD.StencilWriteMask = 0xFF;
		descDSD.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		descDSD.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		descDSD.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		descDSD.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		descDSD.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		descDSD.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		descDSD.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		descDSD.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		d3dDevice->CreateDepthStencilState(&descDSD, &depthStencilState);

		d3dContext->OMSetDepthStencilState(depthStencilState, 1);
	}

	void TurnOffDepth()
	{
		D3D11_DEPTH_STENCIL_DESC descDDSD;
		ZeroMemory(&descDDSD, sizeof(descDDSD));
		descDDSD.DepthEnable = false;
		descDDSD.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		descDDSD.DepthFunc = D3D11_COMPARISON_LESS;
		descDDSD.StencilEnable = true;
		descDDSD.StencilReadMask = 0xFF;
		descDDSD.StencilWriteMask = 0xFF;
		descDDSD.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		descDDSD.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		descDDSD.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		descDDSD.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		descDDSD.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		descDDSD.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		descDDSD.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		descDDSD.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		d3dDevice->CreateDepthStencilState(&descDDSD, &depthStencilDisabledState);
		d3dContext->OMSetDepthStencilState(depthStencilDisabledState, 1);
	}


};
#endif