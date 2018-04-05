////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"


GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_minimapCamera = 0;
	m_Shader = 0; 

	m_Light = 0;

	m_Model_Circle = 0;
	m_Model_Plane = 0;
	m_Model_Plane2 = 0;

	m_Model_Cube = 0;

	m_Model_Cube3 = 0;
	m_Model_CircleList = 0;

	m_Model_Mirror = 0;

	m_2D_Love = 0;
	m_Loading = 0;
	m_CrossHair = 0;
	m_Cursor = 0;

	m_Title = 0;

	m_Frustum = 0;

	m_RTTTexture = 0;

	m_Terrain = 0;
	m_TerrainShader = 0;
	//m_QuadTree = 0;

	m_Sky = 0;


	F1pressed = false;

	CharacterPos = { 946.0f, 56.0f, 464.0f };
	CharacterRot = { 0.0f, 0.0f, 0.0f };

	m_fbx = 0;


	//호수
	m_Water = 0;
	//m_ReflectionTexture = 0;
	m_RefractionTexture = 0;
	m_WaterTerrain = 0;
	m_WaterTerrainShader = 0;

	//불
	m_Fire_Effect = 0;
	frameTime = 0.0f;
	m_Particle = 0;
	m_Particleactive = false;

	//빌보드
	m_Billboard_Tree = 0;

	//미니맵
	m_Minimap = 0;

	//인스턴싱
	m_Instancing = 0;

}

GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}
GraphicsClass::~GraphicsClass()
{
}


/*----------------------------------------------------------------------------------------
이름 : Initialize()
용도 : D3D객체, 카메라, 메인화면 객체 초기화
------------------------------------------------------------------------------------------*/
bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;


	// D3D객체 생성
	//-------------------------------------------------------------------------------------
	m_D3D = new D3DClass;

	if (!m_D3D) { return false; }

	/* Direct3D 객체를 초기화합니다.
	 이 함수에서 화면의 너비, 높이, 윈도우 핸들, Graphicsclass.h에 정의된 네 개의 전역 변수들을 전달한다.
	 D3DCLASS에서 이 변수들을 사용하여 Direct3D 시스템을 설정한다. */
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result) { MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK); return false; }

	//--------------------------------------------------------------------------------------

	// 카메라객체 생성
	//-------------------------------------------------------------------------------------
	m_Camera = new CameraClass;

	if (!m_Camera) { return false; }

	//글꼴의 위치
	m_Camera->SetPosition(D3DXVECTOR3(0.0f, 0.0f, -1.0f));
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);
	//-------------------------------------------------------------------------------------


	// 셰이더 객체 생성
	//-------------------------------------------------------------------------------------
	m_Shader = new ShaderManagerClass;
	if (!m_Shader) { return false; }

	result = m_Shader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) { MessageBox(hwnd, L"Could not initialize the color shader object", L"Error", MB_OK); return false; }
	//-------------------------------------------------------------------------------------


	// 메인이미지 객체 생성
	//-------------------------------------------------------------------------------------
	m_2D_Love = new ImageClass;

	result = m_2D_Love->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, L"../Dreamy/Main.jpg", screenWidth, screenHeight);
	if (!result) { MessageBox(hwnd, L"Could not initialize model object", L"Error", MB_OK); return false; }
	//-------------------------------------------------------------------------------------


	return true;
}


/*----------------------------------------------------------------------------------------
이름 : Loading()
용도 : 실제로 렌더링 되는 애들 초기화
------------------------------------------------------------------------------------------*/
bool GraphicsClass::Loading(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	// 로딩이미지 객체 생성
	//-------------------------------------------------------------------------------------
	m_Loading = new ImageClass;

	result = m_Loading->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, L"../Dreamy/Loading.jpg", screenWidth, screenHeight);
	if (!result) { MessageBox(hwnd, L"Could not initialize model object", L"Error", MB_OK); return false; }
	//-------------------------------------------------------------------------------------

	// 로딩이미지 출력
	RenderLoadingScene();


	// 카메라 객체 생성
	//-------------------------------------------------------------------------------------
	m_minimapCamera = new CameraClass;
	if (!m_minimapCamera) { return false; }

	//미니맵 카메라 
	m_minimapCamera->SetPosition(D3DXVECTOR3(0.0f, 0.0f, -2.5f));
	m_minimapCamera->SetRotation(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	//-------------------------------------------------------------------------------------



	// Text객체 생성
	//-------------------------------------------------------------------------------------
	m_Title = new TextClass;

	if (!m_Title) { return false; }

	result = m_Title->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix);
	if (!result) { MessageBox(hwnd, L"Could not initialize TitleText", L"Error", MB_OK); return false; }
	//-------------------------------------------------------------------------------------

	// 빌보드 객체 생성(이펙트,나무 등)
	//-------------------------------------------------------------------------------------
	m_Particle = new ParticleSystem;
	if (!m_Particle) { return false; }

	result = m_Particle->Initialize(m_D3D->GetDevice(), L"../Dreamy/Data/star.dds");
	if (!result) { MessageBox(hwnd, L"Could not particle TitleText", L"Error", MB_OK); return false; }
	
	m_Fire_Effect = new ModelClass;

	result = m_Fire_Effect->InitializeTriple(m_D3D->GetDevice(), "../Dreamy/Data/square.txt", L"../Dreamy/Data/fire01.dds", L"../Dreamy/Data/noise01.dds", L"../Dreamy/Data/alpha01.dds");
	if (!result) { MessageBox(hwnd, L"Could not initialize fire effect object", L"Error", MB_OK); return false; }

	m_Billboard_Tree = new ModelClass;

	result = m_Billboard_Tree->Initialize(m_D3D->GetDevice(), "../Dreamy/Data/square2.txt", L"../Dreamy/Data/tree.png");
	if (!result) { MessageBox(hwnd, L"Could not initialize tree object", L"Error", MB_OK); return false; }


	//-------------------------------------------------------------------------------------

	// 3D모델 객체 생성
	//-------------------------------------------------------------------------------------
	m_Model_Circle = new ModelClass;
	
	result = m_Model_Circle->Initialize(m_D3D->GetDevice(), "../Dreamy/sphere.txt", L"../Dreamy/seafloor.dds");
	if (!result) { MessageBox(hwnd, L"Could not initialize model object", L"Error", MB_OK); return false; }
	
	//m_Model_Plane = new ModelClass;
	//
	//result = m_Model_Plane->InitializeTriple(m_D3D->GetDevice(), "../Dreamy/plane01.txt", L"../Dreamy/dirt01.dds", L"../Dreamy/stone01.dds", L"../Dreamy/alpha01.dds");
	//if (!result) { MessageBox(hwnd, L"Could not initialize mo456456del object", L"Error", MB_OK); return false; }
	//
	//
	m_Model_Cube = new ModelClass;
	
	result = m_Model_Cube->InitializeTriple(m_D3D->GetDevice(), "../Dreamy/cube.txt", L"../Dreamy/dirt01.dds", L"../Dreamy/stone01.dds", L"../Dreamy/alpha01.dds");
	if (!result) { MessageBox(hwnd, L"Could not m_Model_Cube1 object", L"Error", MB_OK); return false; }
	

	m_Model_Cube3 = new ModelClass;
	
	result = m_Model_Cube3->InitializeSpecMap(m_D3D->GetDevice(), "../Dreamy/cube.txt", L"../Dreamy/stone02.dds", L"../Dreamy/bump02.dds", L"../Dreamy/spec02.dds");
	if (!result) { MessageBox(hwnd, L"Could not tranlsmad object", L"Error", MB_OK); return false; }
	
	m_Model_Plane2 = new ModelClass;
	
	result = m_Model_Plane2->Initialize(m_D3D->GetDevice(), "../Dreamy/plane01.txt", L"../Dreamy/fog01.png");
	if (!result) { MessageBox(hwnd, L"Could not initialize fogplane object", L"Error", MB_OK); return false; }
	
	//m_Model_Mirror = new ModelClass;
	//
	//result = m_Model_Mirror->Initialize(m_D3D->GetDevice(), "../Dreamy/floor.txt", L"../Dreamy/blue01.dds");
	//if (!result) { MessageBox(hwnd, L"Could not initialize Mirror object", L"Error", MB_OK); return false; }

	//-------------------------------------------------------------------------------------

	// Frustum 객체 생성
	//-------------------------------------------------------------------------------------
	m_Frustum = new FrustumClass;
	if (!m_Frustum) { return false; }
	//-------------------------------------------------------------------------------------


	//Terrain, sky, 호수 객체 생성
	//-------------------------------------------------------------------------------------	

	m_Terrain = new TerrainClass;
	if (!m_Terrain) { return false; }

	result = m_Terrain->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), "../Dreamy/setup.txt", "../Dreamy/dirt01d.tga", "../Dreamy/dirt01n.tga");
	if (!result) { MessageBox(hwnd, L"Could not initialize Terrain object", L"Error", MB_OK); return false; }


	m_TerrainShader = new TerrainShaderClass;
	if (!m_TerrainShader) { return false; }

	m_TerrainShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) { MessageBox(hwnd, L"Could not initialize TerrainShader object", L"Error", MB_OK); return false; }

	m_Sky = new SkyClass;
	if (!m_Sky) { return false; }

	result = m_Sky->InitializeSky(m_D3D->GetDevice());
	if (!result) { MessageBox(hwnd, L"Could not initialize Sky object", L"Error", MB_OK); return false; }

	result = m_Sky->InitializeCloud(m_D3D->GetDevice(), L"../Dreamy/cloud001.dds", L"../Dreamy/cloud002.dds");
	if (!result) { MessageBox(hwnd, L"Could not initialize Cloud object", L"Error", MB_OK); return false; }

	//-------------------------------------------------------------------------------------

	// 호수
	//-------------------------------------------------------------------------------------
	// Create the refraction render to texture object.
	m_RefractionTexture = new RTTTextureClass;
	if (!m_RefractionTexture) {return false;}

	// Initialize the refraction render to texture object.
	result = m_RefractionTexture->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result){ MessageBox(hwnd, L"Could not initialize the refraction render to texture object.", L"Error", MB_OK); return false; }

	// Create the water object.
	m_Water = new WaterClass;
	if (!m_Water) {return false;}

	// Initialize the water object.
	result = m_Water->Initialize(m_D3D->GetDevice(), L"../Dreamy/Data/waternormal.dds", 14.0f, 58.0f);
	if (!result) { MessageBox(hwnd, L"Could not initialize the water object.", L"Error", MB_OK); return false;}

	m_WaterTerrain = new TerrainClass;
	if (!m_WaterTerrain) { return false; }

	result = m_WaterTerrain->Initializeforwater(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), "../Dreamy/setup.txt", "../Dreamy/dirt01d.tga", "../Dreamy/dirt01n.tga");
	if (!result) { MessageBox(hwnd, L"Could not initialize m_WaterTerrain object", L"Error", MB_OK); return false; }

	m_WaterTerrainShader = new TerrainShaderClass;
	if (!m_WaterTerrainShader) { return false; }

	m_WaterTerrainShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) { MessageBox(hwnd, L"Could not initialize m_WaterTerrainShader object", L"Error", MB_OK); return false; }
	//-------------------------------------------------------------------------------------

	// 빛 객체 생성
	//-------------------------------------------------------------------------------------
	m_Light = new LightClass;
	if (!m_Light) { return false; }

	m_Light->SetAmbientColor(0.05f, 0.05f, 0.05f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(-0.5f, -1.0f, -0.5f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(16.0f); // 반사강도값이 낮을수록 반사광 효과가 커진다!
	//-------------------------------------------------------------------------------------

    // ModelList객체 생성
    //-------------------------------------------------------------------------------------
	m_Model_CircleList = new ModelManagerClass;
	if (!m_Model_CircleList) { return false; }
	
	// 구 4개를 만들자!
	m_Model_CircleList->Initialize(5);
	if (!result) { MessageBox(hwnd, L"리스트 만드는데 실패함", L"Error", MB_OK); return false; }

	//-------------------------------------------------------------------------------------


	// RTT 객체 생성
	//-------------------------------------------------------------------------------------
	//m_RTT = new RTTClass;
	//if (!m_RTT) { return false; }
	//
	//// 모든 화면을 텍스처에 그리고 싶기 때문에 화면의 너비와 높이를 텍스쳐의 크기로 지정했디.
	//
	//result = m_RTT->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight);
	//if (!result) { MessageBox(hwnd, L"리스트 만드는데 실패함", L"Error", MB_OK); return false; }
	//-------------------------------------------------------------------------------------

	// RTT 텍스처(거울, 물 표면) 객체 생성
	//-------------------------------------------------------------------------------------

	// 인스턴싱 객체들 생성
	//-------------------------------------------------------------------------------------
	m_Instancing = new InstancingClass;
	if (!m_Instancing) { return false; }

	m_Instancing->SetInstanceCount(4);
	m_Instancing->SetInstancePosition(659.0f, 48.0f, 784.0f);
	
	result = m_Instancing->Initialize(m_D3D->GetDevice(), "../Dreamy/Data/square2.txt", L"../Dreamy/Data/redtree.png");
	if(!result) { MessageBox(hwnd, L"instancing", L"Error", MB_OK); return false; }

	//-------------------------------------------------------------------------------------

	// FBX모델 생성
	//-------------------------------------------------------------------------------------
	m_fbx = new FBXModel;
	if (!m_fbx) { return false; }
	
	result = m_fbx->Initialize(m_D3D->GetDevice(), "../Dreamy/Data/Horse.fbx", L"../Dreamy/Data/Horse_D.png");
	if(!result) { MessageBox(hwnd, L"fbx", L"Error", MB_OK); return false; }



	//-------------------------------------------------------------------------------------

	// 2D이미지 생성
	//-------------------------------------------------------------------------------------
	m_CrossHair = new ImageClass;
	if (!m_CrossHair) { return false; }

	result = m_CrossHair->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, L"../Dreamy/crosshair.png", screenWidth,screenHeight);
	if(!result) { MessageBox(hwnd, L"crosshair error", L"Error", MB_OK); return false; }

	m_Cursor = new ImageClass;
	if (!m_Cursor) { return false; }

	result = m_Cursor->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, L"../Dreamy/Data/mouse.dds", 32, 32);
	if (!result) { MessageBox(hwnd, L"m_Cursor error", L"Error", MB_OK); return false; }
	//-------------------------------------------------------------------------------------

	// 미니맵
	//-------------------------------------------------------------------------------------
	m_Minimap = new MinimapClass;
	if (!m_Minimap) { return false; }

	result = m_Minimap->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, 1025, 1025);
	if (!result) { MessageBox(hwnd, L"minimap error", L"Error", MB_OK); return false; }
	//-------------------------------------------------------------------------------------

	return true;
}

// 모든 그래픽 객체의 종료가 여기에서 일어난다.
void GraphicsClass::Shutdown()
{
	
	if (m_Title)	{ m_Title->Shutdown();	delete m_Title;	m_Title = 0;}
	if (m_2D_Love)	{ m_2D_Love->Shutdown(); delete m_2D_Love; m_2D_Love = 0;}
	if (m_CrossHair) { m_CrossHair->Shutdown(); delete m_CrossHair; m_CrossHair = 0; }
	if (m_Cursor) { m_Cursor->Shutdown(); delete m_Cursor; m_Cursor = 0; }
	if (m_Billboard_Tree) { m_Billboard_Tree->Shutdown(); delete m_Billboard_Tree; m_Billboard_Tree = 0; }
	if (m_Instancing) { m_Instancing->Shutdown(); delete m_Instancing; m_Instancing = 0; }
	//if (m_QuadTree) { m_QuadTree->Shutdown(); delete m_QuadTree; m_QuadTree = 0; }
	// Release the texture manager object.
	if (m_Minimap) { m_Minimap->Shutdown(); delete m_Minimap; m_Minimap = 0; }
	if (m_Water) { m_Water->Shutdown(); delete m_Water; m_Water = 0; }
	//if (m_ReflectionTexture) { m_ReflectionTexture->Shutdown(); delete m_ReflectionTexture; m_ReflectionTexture = 0; }
	if (m_RefractionTexture) { m_RefractionTexture->Shutdown(); delete m_RefractionTexture; m_RefractionTexture = 0; }
	if (m_TerrainShader) { m_TerrainShader->Shutdown(); delete m_TerrainShader; m_TerrainShader = 0; }
	if (m_WaterTerrainShader) { m_WaterTerrainShader->Shutdown(); delete m_WaterTerrainShader; m_WaterTerrainShader = 0; }
	if (m_Loading) { m_Loading->Shutdown(); delete m_Loading; m_Loading = 0; }
	//if (m_RTT) { m_RTT->Shutdown(); delete m_RTT; m_RTT = 0; }
	if (m_RTTTexture) { m_RTTTexture->Shutdown(); delete m_RTTTexture; m_RTTTexture = 0; }
	if (m_Frustum) { delete m_Frustum; m_Frustum = 0; }
	if (m_fbx) { m_fbx->ShutDown(); delete m_fbx; m_fbx = 0; }
	if (m_Particle) { m_Particle->Shutdown(); delete m_Particle; m_Particle = 0; }
	if (m_Terrain) { m_Terrain->Shutdown(); delete m_Terrain; m_Terrain = 0; }
	if (m_WaterTerrain) { m_WaterTerrain->Shutdownforwater(); delete m_WaterTerrain; m_WaterTerrain = 0; }
	if (m_Sky) { m_Sky->Shutdown(); delete m_Sky; m_Sky = 0; }
	if (m_Model_CircleList) { m_Model_CircleList->Shutdown(); delete m_Model_CircleList; m_Model_CircleList = 0; }
	if (m_Model_Cube) { m_Model_Cube->Shutdown(); delete m_Model_Cube; m_Model_Cube = 0; }
	if (m_Fire_Effect) { m_Fire_Effect->Shutdown(); delete m_Fire_Effect; m_Fire_Effect = 0; }
	if (m_Model_Cube3) { m_Model_Cube3->Shutdown(); delete m_Model_Cube3; m_Model_Cube3 = 0; }
	if (m_Model_Mirror) { m_Model_Mirror->Shutdown(); delete m_Model_Mirror; m_Model_Mirror = 0; }
	if (m_Light)	{ delete m_Light; m_Light = 0; }
	if (m_Shader)	{ m_Shader->Shutdown(); delete m_Shader; m_Shader = 0;}
	if (m_Model_Circle)	{ m_Model_Circle->Shutdown(); delete m_Model_Circle;m_Model_Circle = 0;}
	if (m_Model_Plane)	{ m_Model_Plane->Shutdown(); delete m_Model_Plane;m_Model_Plane = 0;}
	if (m_Model_Plane2) { m_Model_Plane2->Shutdown(); delete m_Model_Plane2; m_Model_Plane2 = 0; }
	if (m_Camera)	{ delete m_Camera; m_Camera = 0;}
	if (m_minimapCamera) { delete m_minimapCamera; m_minimapCamera = 0; }
	if (m_D3D){ m_D3D->Shutdown(); delete m_D3D; m_D3D = 0; }
	
	return;
}

/*----------------------------------------------------------------------------------------
이름 : Frame()
용도 : 
- SystemClass에서 프레임 마다 호출된다.
- 마우스포인터 움직이는것 할 수 있음
- fps,cpu,frametime재서 text로 출력.
- 물체 돌릴 수 있음
- 각 프레임 마다 구름의 변환.
- 각 프레임 마다 호수의 변환과 호수 표면 텍스처의 3단계 RTT <- 이게 너무 비싼 기능임.
------------------------------------------------------------------------------------------*/
// 매 호출마다 Render함수를 부른다.
bool GraphicsClass::Frame(int fps, int cpu, float frameTime, D3DXVECTOR3 pos, D3DXVECTOR3 rot, int mouseX, int mouseY)
{
	bool result, foundHeight;
	D3DXVECTOR3 CameraPos;
	float height, Characterheight;

	MousePosX = mouseX;
	MousePosY = mouseY;

	// fps세팅
	result = m_Title->SetFps(fps, m_D3D->GetDeviceContext());
	if (!result) { return false; }


	// cpu세팅
	result = m_Title->SetCpu(cpu, m_D3D->GetDeviceContext());
	if (!result) { return false; }


	//m_Camera->SetRotation(rot);


	 //높이 기반 카메라 이동을 위함
	//foundHeight = m_Terrain->GetHeightAtPosition(pos.x, pos.z, height);
	//if (foundHeight)
	//{
	//	m_Camera->SetPosition(D3DXVECTOR3(pos.x, height + 5.0f, pos.z));
	//}
	//
	//else
	//{
	//	m_Camera->SetPosition(pos);
	//}

	//캐릭터 위치 초기화
	//CharacterPos.x = pos.x;
	//CharacterPos.z = pos.z+5.5f;
	//
	////캐릭터 회전 초기화
	//CharacterRot = D3DXVECTOR3(rot.x, rot.y, rot.z);

	CameraPos = pos;
	
	//높이 기반 캐릭터 이동을 위함
	//foundHeight = m_Terrain->GetHeightAtPosition(CharacterPos.x, CharacterPos.z, Characterheight);
	//if (foundHeight)
	//{
	//	CharacterPos.y = Characterheight + 30.0f;
	//
	//}

	foundHeight = m_Terrain->GetHeightAtPosition(CameraPos.x, CameraPos.z, Characterheight);
	if (foundHeight)
	{
		CameraPos.y = Characterheight + 30.0f;

	}
	//카메라 위치 초기화
	//m_Camera->SetPosition(D3DXVECTOR3(CharacterPos.x, CharacterPos.y, CharacterPos.z - 0.1f));
	m_Camera->SetPosition(CameraPos);
	//m_Camera->SetPosition(D3DXVECTOR3(512.5f, 30.0f, 300.0f));
	//m_Camera->SetRotation(D3DXVECTOR3(16.0f+rot.x, 0, 0));
	m_Camera->SetRotation(rot);

	//잔물결 이동하는 속도
	m_Water->Frame(frameTime);

	//구름의 프레임 처리를 수행한다.
	m_Sky->Frame(frameTime*0.00001f, 0.0f, frameTime*0.00002f, 0.0f);

	//미니맵
	m_Minimap->PositionUpdate(CameraPos.x, CameraPos.z);

	//파티클
	m_Particle->Frame(frameTime, m_D3D->GetDeviceContext());
	return true;
}


/*----------------------------------------------------------------------------------------
이름 : Render()
용도 : 러닝씬과 RTT씬을 그린다.
- 반사를 할 때는 우선 RTT를 먼저하고 러닝씬을 로드해야 혼합된다.
------------------------------------------------------------------------------------------*/
bool GraphicsClass::Render( bool Pressed, int frametime)
{
	bool result;

	//RTT시작
	//-------------------------------------------------------------------------------------
	//result = RenderToTexture(); // 거울
	//if (!result) { return false; }
	//if (frametime%15==0)
	//{
	////호수의 굴절 텍스처
	result = RenderRefractionToTexture(Pressed);
	if (!result) { return false; }
	//}
	//호수의 반사 텍스처
	//result = RenderReflectionToTexture();
	//if (!result) { return false; }
	//-------------------------------------------------------------------------------------

	// 메인씬 그림
	//-------------------------------------------------------------------------------------
	result = RenderRunningScene(Pressed);
	if (!result) { return false; }
	//-------------------------------------------------------------------------------------
	
	return true;
}

/*----------------------------------------------------------------------------------------
이름 : RenderRunningScene()
용도 :
- 1. 카메라의 위치를 토대로 뷰 행렬을 만들기 위해 카메라의 render함수를 호출한다.
- 2. 뷰 행렬을 만들고 복사본을 가져온다.
- 3. d3dclass객체로 부터 월드 행렬과 투영 행렬을 복사해온다.
- 4. modelclass의 render()함수를 호출하여 그래픽 파이프라인에 삼각형 모델을 그리도록 한다.
- 5. 여기서 준비한 정점들로 셰이더를 호출하여 셰이더는 모델 정보와 정점을 배치시키기 위한 세 행렬을 사용하여 정점들을 그려낸다.
- 6. 삼각형이 백버퍼에 그려진다.

- ModelClass, ImageClass, TextClass의 Render()은 정점/인덱스 버퍼를 그래픽스 파이프라인에 묶는 I/A단계 까지만 한다.
- 이 기하구조를 그리는(출력하는)건 ShaderClass의 Render()이다!

- 안개 구현 시 백버퍼를 안개의 색으로 지우는것은 매우 중요한 부분이다.
- FogEnd가 멀어질수록(+될수록) 밝아진다 = 물체의 위치(정점셰이더에서 cameraposition)가 FogENd와 가까워질수록(+) FogFactor가 커져서 텍스처가 가려진다.

안개는 ...나중에 ... 높이맵 했을 때 높이맵에만 셰이더를 씌우는게 나을듯.
------------------------------------------------------------------------------------------*/
bool GraphicsClass::RenderRunningScene(bool Pressed)
{
	
	float fogColor, fogStart, fogEnd;
	//float pitch = CharacterRot.x * 0.0174532925f;
	//float yaw = CharacterRot.y * 0.0174532925f;
	//float roll = CharacterRot.z * 0.0174532925f;
	D3DXVECTOR3 cameraPosition;


	D3DXMATRIX  TerrainworldMatrix, SkyworldMatrix, WaterworldMatrix, PlaneworldMatrix,  MirrorworldMatrix;
	D3DXMATRIX CrossHairworldMatrix;
	D3DXMATRIX FBXworldMatrix, FBXRotationMatrix;
	D3DXMATRIX  TranslationMatrix2, Cube3RotationMatrix;
	D3DXMATRIX viewMatrix, projectionMatrix, orthoMatrix, reflectionMatrix,WaterreflectionViewMatrix;
	D3DXMATRIX ScaleMatrix;

	D3DXMATRIX TextworldMatrix;

	int CircleCount, Circleindex;
	float positionX, positionY, positionZ, radius;
	D3DXVECTOR4 color; // CircleDiffuseColor
	bool Circlerender, result;

	// 텍스처 이동
	//-------------------------------------------------------------------------------------
	static float textureTranslation = 0.0f;
	textureTranslation += 0.003f;
	if (textureTranslation > 1.0f) { textureTranslation -= 1.0f; }
	//-------------------------------------------------------------------------------------

	// 이펙트 변수 설정
	//-------------------------------------------------------------------------------------
	SetEffectVariable();
	//-------------------------------------------------------------------------------------

	//안개 객체 초기화
	//fogEnd가 멀어질수록 밝아짐
	//-------------------------------------------------------------------------------------
	fogColor = 0.0f;
	fogStart = 250.0f;
	fogEnd = 550.0f;
	//-------------------------------------------------------------------------------------

	//시작
	//-------------------------------------------------------------------------------------
	m_D3D->BeginScene(fogColor, fogColor, fogColor, 1.0f);

	//카메라 위치
	//-------------------------------------------------------------------------------------
	m_Camera->Render();
	cameraPosition = m_Camera->GetPosition();

	//m_Camera->RenderWaterReflection(m_Water->GetWaterHeight());

	//기본 월드,뷰,투영,정사영 행렬 획득
	//-------------------------------------------------------------------------------------
	m_Camera->GetViewMatrix(viewMatrix);
	//reflectionMatrix = m_Camera->GetReflectionViewMatrix();

	//m_Camera->GetViewMatrix(RotationMatrix);
	//m_Camera->GetBaseViewMatrix(baseViewMatrix);

	m_D3D->GetWorldMatrix(TerrainworldMatrix);
	m_D3D->GetWorldMatrix(WaterworldMatrix);
	m_D3D->GetWorldMatrix(SkyworldMatrix);
	m_D3D->GetWorldMatrix(PlaneworldMatrix);
	m_D3D->GetWorldMatrix(FBXworldMatrix);
	m_D3D->GetWorldMatrix(FBXRotationMatrix);
	m_D3D->GetWorldMatrix(MirrorworldMatrix);
	m_D3D->GetWorldMatrix(CrossHairworldMatrix);
	m_D3D->GetWorldMatrix(TranslationMatrix2);
	m_D3D->GetWorldMatrix(TextworldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

	//기본 행렬 변환
	//-------------------------------------------------------------------------------------
	// 스케일링->회전->이동 순으로 합쳐야 한다.
	D3DXMatrixTranslation(&SkyworldMatrix, cameraPosition.x, cameraPosition.y, cameraPosition.z);
	


	//-------------------------------------------------------------------------------------

	//시야 절두체 생성
	//-------------------------------------------------------------------------------------
	m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);
	//-------------------------------------------------------------------------------------

	// Sky
	//-------------------------------------------------------------------------------------
	m_D3D->TurnOffCulling();
	m_D3D->TurnZBufferOff();

	m_Sky->RenderSky(m_D3D->GetDeviceContext());
	m_Shader->RenderSkydomeShader(m_D3D->GetDeviceContext(), m_Sky->GetIndexCount(), SkyworldMatrix, viewMatrix, projectionMatrix,
		m_Sky->GetApexColor(), m_Sky->GetCenterColor());

	// Turn back face culling back on.
	m_D3D->TurnOnCulling();

	//구름
	m_D3D->EnableSecondBlendState();

	m_Sky->RenderCloud(m_D3D->GetDeviceContext());

	m_Shader->RenderCloudShader(m_D3D->GetDeviceContext(), m_Sky->GetCloudIndexCount(), SkyworldMatrix, viewMatrix, projectionMatrix,
		m_Sky->GetCloudTexture1(), m_Sky->GetCloudTexture2(), m_Sky->GetTranslation(0), m_Sky->GetTranslation(1),
		m_Sky->GetTranslation(2), m_Sky->GetTranslation(3), m_Sky->GetBrightness());

	
	m_D3D->TurnOffAlphaBlending();

	// Turn the Z buffer back on.
	m_D3D->TurnZBufferOn();
	//-------------------------------------------------------------------------------------


	// Terrain
	//-------------------------------------------------------------------------------------
	
	// Terrain에서 정한 셀 갯수만큼 셀을 로드한다.
	for (int i = 0; i < m_Terrain->GetCellCount(); i++)
	{
		// Put the terrain cell buffers on the pipeline.
		result = m_Terrain->RenderCell(m_D3D->GetDeviceContext(), i, m_Frustum);
		if (result)
		{

		result = m_TerrainShader->Render(m_D3D->GetDeviceContext(), m_Terrain->GetCellIndexCount(i), TerrainworldMatrix, viewMatrix,
			projectionMatrix, m_Terrain->GetColorTexture(), m_Terrain->GetNormalMapTexture(), m_Light->GetDirection(), m_Light->GetDiffuseColor(), fogStart, fogEnd);
		if (!result) { return false; }

			//m_Terrain->RenderCellLines(m_D3D->GetDeviceContext(), i);
			//m_Shader->RenderColorShader(m_D3D->GetDeviceContext(), m_Terrain->GetCellLinesIndexCount(i), TerrainworldMatrix, viewMatrix, projectionMatrix);
			//if (!result) { return false; }
		}
	}


	// 호수  512.5f, 30.0f, 310.0f
	//-------------------------------------------------------------------------------------
	m_Camera->RenderWaterReflection(m_Water->GetWaterHeight());
	m_Camera->GetWaterReflectionViewMatrix(WaterreflectionViewMatrix);
	D3DXMATRIX WaterRotationMatrix;
	D3DXMatrixTranslation(&WaterworldMatrix, 722.0f, m_Water->GetWaterHeight(), 688.0f);
	//D3DXMatrixRotationY(&WaterRotationMatrix, 70.0f);
	//3DXMatrixMultiply(&WaterworldMatrix, &WaterRotationMatrix, &WaterworldMatrix);
	m_Water->Render(m_D3D->GetDeviceContext());
	
	result = m_Shader->RenderWaterShader(m_D3D->GetDeviceContext(), m_Water->GetIndexCount(), WaterworldMatrix, viewMatrix, projectionMatrix, WaterreflectionViewMatrix,
		m_RefractionTexture->GetShaderResourceView(),/* m_Sky->GetCloudTexture2(),*/ m_Water->GetTexture(),
		m_Camera->GetPosition(), m_Water->GetNormalMapTiling(), m_Water->GetWaterTranslation(), m_Water->GetReflectRefractScale(),
		m_Water->GetRefractionTint(), m_Light->GetDirection(), m_Water->GetSpecularShininess());
	if (!result) { return false; }
	//-------------------------------------------------------------------------------------
	
	//구 리스트 변수들 초기화
	//-------------------------------------------------------------------------------------
	CircleCount = m_Model_CircleList->GetModelCount();
	//-------------------------------------------------------------------------------------
	
	// Scene 출력 모델
	//-------------------------------------------------------------------------------------
	
	//프러스텀 컬링 들어간 구 모델 리스트
	D3DXMATRIX CircleWorldMatrix;
	m_D3D->GetWorldMatrix(CircleWorldMatrix);
	m_Model_Circle->Scale(1.5f, 1.5f, 1.5f);
	
	for (Circleindex = 0; Circleindex < CircleCount; Circleindex++)
	{
		//리스트에 있는것들 차례대로 데이터를 읽어온다.
		m_Model_CircleList->GetData(Circleindex, positionX, positionY, positionZ, color);
	
		radius = 1.0f;
		//755.0f, 26.0f, 389.0f
		//Frustum Culling(프러스텀 컬링)
		//3D모델에 경계볼륨을 씌워서 절두체에 보이는지 체크한다.
		Circlerender = m_Frustum->CheckSphere(positionX + 730.0f, positionY + 26.0f, positionZ + 375.0f, radius);
	
		if (Circlerender)
		{
			//리스트에서 정의된대로 위치 변환
			m_Model_Circle->Translation(positionX + 730.0f, positionY + 26.0f, positionZ + 375.0f);
			m_Model_Circle->Multiply(m_Model_Circle->GetScailingMatrix(), m_Model_Circle->GetTranslationMatrix());
			D3DXMatrixMultiply(&CircleWorldMatrix, &m_Model_Circle->GetFinalMatrix(), &CircleWorldMatrix);
	
			//3D모델(구체) 렌더링
			m_Model_Circle->Render(m_D3D->GetDeviceContext());
	
			//셰이더 렌더링
			result = m_Shader->RenderLightShader(m_D3D->GetDeviceContext(), m_Model_Circle->GetIndexCount(), CircleWorldMatrix, viewMatrix, projectionMatrix
				, m_Model_Circle->GetTexture(),m_Light->GetDirection(), m_Light->GetAmbientColor(), color
				, m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
			if (!result) { return false; }
	
			//구가 각자 다른 위치를 가져야 하니까 reset시킨다!
			m_D3D->GetWorldMatrix(CircleWorldMatrix);
			
		}
	}
	
	//큐브1 알파맵
	D3DXMATRIX Cube1WorldMatrix;
	m_D3D->GetWorldMatrix(Cube1WorldMatrix);
	
	m_Model_Cube->Translation(750.0f, 26.0f, 380.0f);
	m_Model_Cube->Multiply(m_Model_Cube3->GetRotationYMatrix(), m_Model_Cube->GetTranslationMatrix());
	m_Model_Cube->Multiply(m_Model_Cube3->GetScailingMatrix(), m_Model_Cube->GetFinalMatrix());
	D3DXMatrixMultiply(&Cube1WorldMatrix, &m_Model_Cube->GetFinalMatrix(), &Cube1WorldMatrix);
	
	m_Model_Cube->Render(m_D3D->GetDeviceContext());
	
	result = m_Shader->RenderAlphaMapShader(m_D3D->GetDeviceContext(), m_Model_Cube->GetIndexCount(), Cube1WorldMatrix, viewMatrix, projectionMatrix
		, m_Model_Cube->GetTripleTextureArray());
	if (!result) { return false; }
	
	
	//큐브3 범프맵+스페큘러맵+텍스처이동
	D3DXMATRIX Cube3WorldMatrix;
	m_D3D->GetWorldMatrix(Cube3WorldMatrix);
	m_Model_Cube3->Translation(767.0f, 26.0f, 389.0f);
	m_Model_Cube3->RotationY(-40.0f);
	m_Model_Cube3->Scale(7.0f, 7.0f, 7.0f);
	m_Model_Cube3->Multiply(m_Model_Cube3->GetRotationYMatrix(), m_Model_Cube3->GetTranslationMatrix());
	m_Model_Cube3->Multiply(m_Model_Cube3->GetScailingMatrix(), m_Model_Cube3->GetFinalMatrix());
	D3DXMatrixMultiply(&Cube3WorldMatrix, &m_Model_Cube3->GetFinalMatrix(), &Cube3WorldMatrix);
	
	m_Model_Cube3->Render(m_D3D->GetDeviceContext());
	
	result = m_Shader->RenderTranslateShader(m_D3D->GetDeviceContext(), m_Model_Cube3->GetIndexCount(), Cube3WorldMatrix, viewMatrix, projectionMatrix
		, m_Model_Cube3->GetTripleTextureArray(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
		m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower(), textureTranslation);
	if (!result) { return false; }	
	
	
	//평면2 투명
	D3DXMATRIX Plane2worldMatrix;
	m_D3D->GetWorldMatrix(Plane2worldMatrix);
	
	m_Model_Plane2->RotationX(D3DXToRadian(90));
	m_Model_Plane2->RotationY(D3DXToRadian(-45));
	m_Model_Plane2->RotationMultiply(m_Model_Plane2->GetRotationXMatrix(), m_Model_Plane2->GetRotationYMatrix());
	m_Model_Plane2->Translation(784.0f, 40.0f, 371.0f);
	
	m_Model_Plane2->Multiply(m_Model_Plane2->GetRotationMatrix(), m_Model_Plane2->GetTranslationMatrix());
	m_Model_Plane2->Multiply(m_Model_Cube3->GetScailingMatrix(), m_Model_Plane2->GetFinalMatrix());
	D3DXMatrixMultiply(&Plane2worldMatrix, &m_Model_Plane2->GetFinalMatrix(), &Plane2worldMatrix);
	
	
	//평면2 투명
	if (sibal == true)
	{
		m_D3D->TurnOnAlphaBlending();
	
		m_Model_Plane2->Render(m_D3D->GetDeviceContext());
	
		result = m_Shader->RenderTransparentShader(m_D3D->GetDeviceContext(), m_Model_Plane2->GetIndexCount(), Plane2worldMatrix, viewMatrix, projectionMatrix
			, m_Model_Plane2->GetTexture(), 0.7f);
		if (!result) { return false; }
	
		m_D3D->TurnOffAlphaBlending();
	}
	
	//빌보드, 이펙트
	//-------------------------------------------------------------------------------------
	
	//파티클
	D3DXVECTOR3 ParticlePosition = { 767.0f, 30.0f, 389.0f };
	D3DXMATRIX ParticleWorldMatrix;
	m_D3D->GetWorldMatrix(ParticleWorldMatrix);
	m_Particle->Translation(ParticlePosition.x, ParticlePosition.y, ParticlePosition.z);
	m_Particle->RotationY(CalculateBillboarding(cameraPosition, ParticlePosition));
	m_Particle->Multiply(m_Particle->GetRotationYMatrix(), m_Particle->GetTranslationMatrix());
	D3DXMatrixMultiply(&ParticleWorldMatrix, &m_Particle->GetFinalMatrix(), &ParticleWorldMatrix);

	if (m_Particle->active == true)
	{
		m_D3D->EnableSecondBlendState();

		m_Particle->Render(m_D3D->GetDeviceContext());

		result = m_Shader->RenderParticleShader(m_D3D->GetDeviceContext(), m_Particle->GetIndexCount(), ParticleWorldMatrix, viewMatrix, projectionMatrix,
			m_Particle->GetTexture());
		if (!result) { return false; }

	}

	//불
	D3DXVECTOR3 firePosition = { 760.0f, 46.0f, 380.0f };
	
	m_D3D->TurnOnAlphaBlending();
	
	D3DXMATRIX FireWorldMatrix;
	m_D3D->GetWorldMatrix(FireWorldMatrix);
	m_Fire_Effect->Translation(firePosition.x, firePosition.y, firePosition.z);
	m_Fire_Effect->RotationY(CalculateBillboarding(cameraPosition,firePosition));
	m_Fire_Effect->Scale(5.0f, 5.0f, 5.0f);
	m_Fire_Effect->Multiply(m_Fire_Effect->GetRotationYMatrix(), m_Fire_Effect->GetTranslationMatrix());
	m_Fire_Effect->Multiply(m_Fire_Effect->GetScailingMatrix(), m_Fire_Effect->GetFinalMatrix());
	D3DXMatrixMultiply(&FireWorldMatrix, &m_Fire_Effect->GetFinalMatrix(), &FireWorldMatrix);
	
	m_Fire_Effect->Render(m_D3D->GetDeviceContext());
	
	result = m_Shader->RenderFireShader(m_D3D->GetDeviceContext(), m_Fire_Effect->GetIndexCount(), FireWorldMatrix, viewMatrix,
		projectionMatrix, m_Fire_Effect->GetTripleTexture1(), m_Fire_Effect->GetTripleTexture2(), m_Fire_Effect->GetTripleTexture3(),
		frameTime, scrollSpeeds, scales, distortion1, distortion2, distortion3, distortionScale, distortionBias);
	if (!result) { return false; }
	


	D3DXMATRIX InstancingWorldMatrix;
	m_D3D->GetWorldMatrix(InstancingWorldMatrix);
	
	
	m_Instancing->Render(m_D3D->GetDeviceContext());
	
	result = m_Shader->RenderInstancingShader(m_D3D->GetDeviceContext(), m_Instancing->GetVertexCount(), m_Instancing->GetInstanceCount(), InstancingWorldMatrix, viewMatrix, projectionMatrix, m_Instancing->GetTexture());
	if (!result) { return false; }
	
	
	D3DXVECTOR3 TreePosition;
	TreePosition = { 740.0f, 35.0f, 612.0f };
	
	D3DXMATRIX TreeWorldMatrix;
	m_D3D->GetWorldMatrix(TreeWorldMatrix);
	
	
	m_Billboard_Tree->Translation(740.0f, 35.0f, 612.0f);
	m_Billboard_Tree->RotationY(CalculateBillboarding(cameraPosition, TreePosition));
	//m_Billboard_Tree->Scale(20.0f, 20.0f, 20.0f);
	m_Billboard_Tree->Multiply(m_Billboard_Tree->GetRotationYMatrix(), m_Billboard_Tree->GetTranslationMatrix());
	//m_Billboard_Tree->Multiply(m_Billboard_Tree->GetScailingMatrix(), m_Billboard_Tree->GetFinalMatrix());
	D3DXMatrixMultiply(&TreeWorldMatrix, &m_Billboard_Tree->GetFinalMatrix(), &TreeWorldMatrix);
	
	m_Billboard_Tree->Render(m_D3D->GetDeviceContext());
	result = m_Shader->RenderTextureShader(m_D3D->GetDeviceContext(), m_Billboard_Tree->GetIndexCount(), TreeWorldMatrix, viewMatrix, projectionMatrix, m_Billboard_Tree->GetTexture());
	if (!result) { return false; }
	
	
	
	
	m_D3D->TurnOffAlphaBlending();
	
	
	
	//-------------------------------------------------------------------------------------
	//2D이미지, 2d셰이더 렌더링, Text, Z버퍼 ON/OFF, 알파블렌딩, RTT미니맵
	//-------------------------------------------------------------------------------------
	m_D3D->TurnZBufferOff();

	m_D3D->TurnOnAlphaBlending();
//	D3DXMATRIX TextScaleMatrix;
//	D3DXMatrixScaling(&TextScaleMatrix, 2.0f, 2.0f, 2.0f);
//	D3DXMatrixMultiply(&TextworldMatrix, &TextScaleMatrix, &TextworldMatrix);
	// Text출력
	// 마우스 포인터위치 잡을 수 있게
	result = m_Title->SetMousePosition(MousePosX, MousePosY, m_D3D->GetDeviceContext());
	result = m_Title->SetPosition(cameraPosition.x, cameraPosition.y, cameraPosition.z, m_D3D->GetDeviceContext());
	result = m_Title->Render(m_D3D->GetDeviceContext(), TextworldMatrix, orthoMatrix);
	if (!result) { return false; }
	result = m_Minimap->Render(m_D3D->GetDeviceContext(), m_Shader->m_TextureShader, CrossHairworldMatrix, baseViewMatrix, orthoMatrix);
	if (!result) { return false; }



	// 크로스헤어 이미지.
	result = m_CrossHair->Render(m_D3D->GetDeviceContext(), 0, 0);
	if (!result) { return false; }
	result = m_Shader->RenderTextureShader(m_D3D->GetDeviceContext(), m_CrossHair->GetIndexCount(), CrossHairworldMatrix, baseViewMatrix, orthoMatrix, m_CrossHair->GetTexture());
	if (!result) { return false; }


	// 마우스 커서
	result = m_Cursor->Render(m_D3D->GetDeviceContext(), MousePosX, MousePosY);  if (!result) { return false; }
	D3DXMATRIX MouseWorldMatrix;
	D3DXMatrixScaling(&MouseWorldMatrix, 1.5f, 1.5f, 1.5f);
	result = m_Shader->RenderTextureShader(m_D3D->GetDeviceContext(), m_Cursor->GetIndexCount(), MouseWorldMatrix, baseViewMatrix, orthoMatrix, m_Cursor->GetTexture());


	m_D3D->TurnOffAlphaBlending();
	m_D3D->TurnZBufferOn();

	// FBX모델
	D3DXMatrixTranslation(&FBXworldMatrix, 946.0f, 40.0f, 464.0f);
	D3DXMatrixScaling(&FBXRotationMatrix, 0.1f, 0.1f, 0.1f);

	D3DXMatrixMultiply(&FBXworldMatrix, &FBXRotationMatrix, &FBXworldMatrix);

	//fbx모델
	m_D3D->TurnOffCulling();
	m_fbx->Render(m_D3D->GetDeviceContext());
	result = m_Shader->RenderTextureShader(m_D3D->GetDeviceContext(), m_fbx->GetIndexCount(), FBXworldMatrix, viewMatrix, projectionMatrix, m_fbx->GetTexture());

	m_D3D->TurnOnCulling();


	//-------------------------------------------------------------------------------------

	/*
	//기본 행렬 변환
	//-------------------------------------------------------------------------------------
	// 스케일링->회전->이동 순으로 합쳐야 한다.


	//거울
	D3DXMatrixTranslation(&MirrorworldMatrix, -2.7, 3.8, -8.9);
	D3DXMATRIX Scale;
	D3DXMatrixScaling(&Scale, 0.2f, 1.0f, 0.2f);
	D3DXMatrixMultiply(&MirrorworldMatrix, &Scale, &MirrorworldMatrix);


	//기본 행렬 변환, 3D모델, 셰이더 렌더링, 프러스텀 컬링
	//-------------------------------------------------------------------------------------

	//거울
	m_Model_Mirror->Render(m_D3D->GetDeviceContext());

	result = m_Shader->RenderReflectionShader(m_D3D->GetDeviceContext(), m_Model_Mirror->GetIndexCount(), MirrorworldMatrix, viewMatrix, projectionMatrix,
		m_Model_Mirror->GetTexture(), m_RTT->GetShaderResourceView(), reflectionMatrix);
	if (!result) { return false; }


	*/

	//버퍼에 그려진 씬을 화면에 표시한다.
	m_D3D->EndScene();
	



	return true;
}

//-------------------------------------------------------------------------------------
//이름: RenderRefractionToTexture()
//용도: 호수 내부의 모습을 RTT로 굴절(Refraction)한다.
//-------------------------------------------------------------------------------------
bool GraphicsClass::RenderRefractionToTexture(bool Pressed)
{
	D3DXVECTOR4 clipPlane;
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	// Setup a clipping plane based on the height of the water to clip everything above it to create a refraction.
	clipPlane = D3DXVECTOR4(0.0f, -1.0f, 0.0f, m_Water->GetWaterHeight() + 0.1f);

	// Set the render target to be the refraction render to texture.
	m_RefractionTexture->SetRenderTarget(m_D3D->GetDeviceContext());

	// Clear the refraction render to texture.
	m_RefractionTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the matrices from the camera and d3d objects.
	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	m_WaterTerrain->Renderforwater(m_D3D->GetDeviceContext());
	result = m_Shader->RenderWaterReflectionShader(m_D3D->GetDeviceContext(), m_WaterTerrain->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_WaterTerrain->GetColorTexture(), m_WaterTerrain->GetNormalMapTexture(), m_Light->GetDiffuseColor(),
		m_Light->GetDirection(), 2.0f, clipPlane);
	if (!result) { return false; }

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_D3D->SetBackBufferRenderTarget();

	// Reset the viewport back to the original.
	m_D3D->ResetViewport();

	return true;
}
//
bool GraphicsClass::RenderReflectionToTexture()
{
	//D3DXVECTOR4 clipPlane;
	//D3DXMATRIX reflectionViewMatrix, worldMatrix, projectionMatrix;
	//D3DXVECTOR3 cameraPosition;
	//bool result;
	//
	//// Setup a clipping plane based on the height of the water to clip everything below it.
	//clipPlane = D3DXVECTOR4(0.0f, 1.0f, 0.0f, -m_Water->GetWaterHeight());
	//
	//// Set the render target to be the reflection render to texture.
	//m_ReflectionTexture->SetRenderTarget(m_D3D->GetDeviceContext());
	//
	//// Clear the reflection render to texture.
	//m_ReflectionTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);
	//
	//// Use the camera to render the reflection and create a reflection view matrix.
	//m_Camera->RenderWaterReflection(m_Water->GetWaterHeight());
	//
	//// Get the camera reflection view matrix instead of the normal view matrix.
	//m_Camera->GetWaterReflectionViewMatrix(reflectionViewMatrix);
	//
	//// Get the world and projection matrices from the d3d object.
	//m_D3D->GetWorldMatrix(worldMatrix);
	//m_D3D->GetProjectionMatrix(projectionMatrix);
	//
	//// Get the position of the camera.
	//cameraPosition = m_Camera->GetPosition();
	//
	//// Invert the Y coordinate of the camera around the water plane height for the reflected camera position.
	//cameraPosition.y = -cameraPosition.y + (m_Water->GetWaterHeight() * 2.0f);
	//
	//// Translate the sky dome and sky plane to be centered around the reflected camera position.
	//D3DXMatrixTranslation(&worldMatrix, cameraPosition.x, cameraPosition.y, cameraPosition.z);
	//
	//m_D3D->TurnOffCulling();
	//m_D3D->TurnZBufferOff();
	//
	//m_Sky->RenderSky(m_D3D->GetDeviceContext());
	//m_Shader->RenderSkydomeShader(m_D3D->GetDeviceContext(), m_Sky->GetIndexCount(), worldMatrix, reflectionViewMatrix, projectionMatrix,
	//	m_Sky->GetApexColor(), m_Sky->GetCenterColor());
	//
	//// Turn back face culling back on.
	//m_D3D->TurnOnCulling();
	//
	////구름
	//m_D3D->EnableSecondBlendState();
	//
	//m_Sky->RenderCloud(m_D3D->GetDeviceContext());
	//
	//
	//m_Shader->RenderCloudShader(m_D3D->GetDeviceContext(), m_Sky->GetCloudIndexCount(), worldMatrix, reflectionViewMatrix, projectionMatrix,
	//	m_Sky->GetCloudTexture1(), m_Sky->GetCloudTexture2(), m_Sky->GetTranslation(0), m_Sky->GetTranslation(1),
	//	m_Sky->GetTranslation(2), m_Sky->GetTranslation(3), m_Sky->GetBrightness());
	//
	//
	//m_D3D->TurnOffAlphaBlending();
	//
	//// Turn the Z buffer back on.
	//m_D3D->TurnZBufferOn();
	//
	//// Reset the world matrix.
	//m_D3D->GetWorldMatrix(worldMatrix);
	//
	//m_WaterTerrain->Renderforwater(m_D3D->GetDeviceContext());
	//result = m_Shader->RenderWaterReflectionShader(m_D3D->GetDeviceContext(), m_WaterTerrain->GetIndexCount(), worldMatrix, reflectionViewMatrix, projectionMatrix,
	//	m_WaterTerrain->GetColorTexture(), m_WaterTerrain->GetNormalMapTexture(), m_Light->GetDiffuseColor(),
	//	m_Light->GetDirection(), 2.0f, clipPlane);
	//if (!result) { return false; }
	//
	//// Reset the render target back to the original back buffer and not the render to texture anymore.
	//m_D3D->SetBackBufferRenderTarget();
	//
	//// Reset the viewport back to the original.
	////m_D3D->ResetViewport();
	//
	return true;
}

/*----------------------------------------------------------------------------------------
이름 : RenderToTexture()
용도 :
- RTT기능을 사용할 수 있게하는 함수
- 평면 반사 기능을 사용하기 위해 반사 뷰 행렬을 생성해야 한다.
------------------------------------------------------------------------------------------*/
bool GraphicsClass::RenderToTexture()
{
	//bool result;
	//D3DXMATRIX worldMatrix, reflectionViewMatrix, projectionMatrix;
	//D3DXMATRIX TranslationMatrix;
	//
	////RTT기능을 초기화시킨다. = 렌더링 타겟을 rtt로 설정한다.
	//m_RTTTexture->SetRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView());
	//
	//// RTT를 초기화 한다.
	//m_RTTTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);
	//
	////// 이 함수에 씬을 그리면 백버퍼 대신 RTT에 렌더링 된다.
	////result = RenderRTTScene();
	////if (!result)
	////{
	////	return false;
	////}
	//
	////카메라가 반사 행렬을 계산하도록 한다. 
	//m_Camera->RenderReflection(3.5f, 4.5f);
	//
	//
	////행렬들
	//reflectionViewMatrix = m_Camera->GetReflectionViewMatrix();
	//m_D3D->GetWorldMatrix(worldMatrix);
	//m_D3D->GetProjectionMatrix(projectionMatrix);
	//D3DXMatrixRotationY(&worldMatrix, 0.6f);
	//D3DXMatrixTranslation(&TranslationMatrix, -3 ,4.5, -8);
	//D3DXMatrixMultiply(&worldMatrix, &worldMatrix, &TranslationMatrix);
	//
	////큐브1
	////-------------------------------------------------------------------------------------------------------------------
	//m_Model_Cube->Render(m_D3D->GetDeviceContext());
	//result = m_Shader->RenderBumpMapShader(m_D3D->GetDeviceContext(), m_Model_Cube->GetIndexCount(), worldMatrix, reflectionViewMatrix, projectionMatrix
	//	, m_Model_Cube->GetMultiTextureArray(), m_Light->GetDirection(), m_Light->GetDiffuseColor());
	//if (!result) { return false; }
	////result = m_Shader->RenderTextureShader(m_D3D->GetDeviceContext(), m_Model_Cube->GetIndexCount(), worldMatrix, reflectionViewMatrix, projectionMatrix
	////	, m_Model_Cube->GetTexture());
	//
	//if (!result) { return false; }
	//
	////-------------------------------------------------------------------------------------------------------------------
	//
	//
	//// 렌더링 타겟을 RTT에서 다시 백버퍼로 돌린다.
	//m_D3D->SetBackBufferRenderTarget();
	//
	return true;
}

/*----------------------------------------------------------------------------------------
이름 : RenderScene()
용도 : RTT를 사용하기 위해 RTT에 그려질 씬
------------------------------------------------------------------------------------------*/
bool GraphicsClass::RenderRTTScene()
{
	//D3DXMATRIX worldMatrix, projectionMatrix, viewMatrix;
	//
	//bool result;
	//
	//m_minimapCamera->Render();
	//m_minimapCamera->GetViewMatrix(minimapMatrix);
	//
	//
	//m_D3D->GetWorldMatrix(worldMatrix);
	//m_D3D->GetProjectionMatrix(projectionMatrix);
	//
	//
	//D3DXMatrixRotationY(&worldMatrix, 0.8f);
	//
	//
	//m_Model_Cube->Render(m_D3D->GetDeviceContext());
	//
	//result = m_Shader->RenderBumpMapShader(m_D3D->GetDeviceContext(), m_Model_Cube->GetIndexCount(), worldMatrix, minimapMatrix, projectionMatrix
	//	, m_Model_Cube->GetMultiTextureArray(), m_Light->GetDirection(), m_Light->GetDiffuseColor());
	//if (!result) { return false; }
	//
	//
	return true;
}

/*----------------------------------------------------------------------------------------
이름 : RenderLoadingScene()
용도 : 로딩화면을 그린다.
------------------------------------------------------------------------------------------*/
bool GraphicsClass::RenderLoadingScene()
{
	bool result;
	D3DXMATRIX MainSceneworldMatrix, orthoMatrix;
	m_D3D->GetOrthoMatrix(orthoMatrix);
	m_D3D->GetWorldMatrix(MainSceneworldMatrix);

	m_D3D->BeginScene(1.0f, 1.0f, 1.0f, 1.0f);

	result = m_Loading->Render(m_D3D->GetDeviceContext(), 0, 0);
	if (!result) { return false; }

	result = m_Shader->RenderTextureShader(m_D3D->GetDeviceContext(), m_Loading->GetIndexCount(), MainSceneworldMatrix, baseViewMatrix, orthoMatrix, m_Loading->GetTexture());
	if (!result) { return false; }


	m_D3D->EndScene();

	return true;
}
/*----------------------------------------------------------------------------------------
이름 : RenderMainScene()
용도 : 메인화면을 그린다.
------------------------------------------------------------------------------------------*/
bool GraphicsClass::RenderMainScene()
{
	bool result;
	D3DXMATRIX MainSceneworldMatrix, orthoMatrix;
	m_D3D->GetOrthoMatrix(orthoMatrix);
	m_D3D->GetWorldMatrix(MainSceneworldMatrix);

	m_D3D->BeginScene(1.0f,1.0f,1.0f,1.0f);

		result = m_2D_Love->Render(m_D3D->GetDeviceContext(), 0, 0);
		if (!result) { return false; }

		result = m_Shader->RenderTextureShader(m_D3D->GetDeviceContext(), m_2D_Love->GetIndexCount(), MainSceneworldMatrix, baseViewMatrix, orthoMatrix, m_2D_Love->GetTexture());
		if (!result) { return false; }
	

	m_D3D->EndScene();

	return true;
}


void GraphicsClass::CheckIntersection(int mouseX, int mouseY, int m_screenWidth, int m_screenHeight)
{
	D3DXMATRIX ProjectionMatrix, ViewMatrix, WorldMatrix;
	m_D3D->GetWorldMatrix(WorldMatrix);
	m_Camera->GetViewMatrix(ViewMatrix);
	m_D3D->GetProjectionMatrix(ProjectionMatrix);

	D3DXVECTOR3 origin;
	origin = m_Camera->GetPosition();

	sibal = m_Model_Cube3->TestIntersection(mouseX, mouseY, m_screenWidth, m_screenHeight, ProjectionMatrix, ViewMatrix, WorldMatrix, origin);
	m_Particle->active = m_Model_Cube3->TestIntersection(mouseX, mouseY, m_screenWidth, m_screenHeight, ProjectionMatrix, ViewMatrix, WorldMatrix, origin);

}

void GraphicsClass::SetEffectVariable()
{
	// 불꽃 이펙트
	//-------------------------------------------------------------------------------------
	frameTime += 0.01f;
	if (frameTime > 1000.0f) { frameTime = 0.0f; }

	// 세 노이즈 텍스쳐의 스크롤 속도를 설정합니다.
	scrollSpeeds = D3DXVECTOR3(1.3f, 2.1f, 2.3f);
	
	// 세 크기값을 사용하여 세 가지의 다른 노이즈 옥타브 텍스쳐를 만듭니다.
	scales = D3DXVECTOR3(1.0f, 2.0f, 3.0f);
	
	// 세 노이즈 텍스쳐의 서로 다른 세 개의 x, y 왜곡 인자를 설정합니다.
	distortion1 = D3DXVECTOR2(0.1f, 0.2f);
	distortion2 = D3DXVECTOR2(0.1f, 0.3f);
	distortion3 = D3DXVECTOR2(0.1f, 0.1f);

	// 텍스쳐 샘플링 좌표의 교란을 위한 크기 및 바이어스 값입니다.
	distortionScale = 0.8f; 
	distortionBias = 0.5f;

}

float GraphicsClass::CalculateBillboarding(D3DXVECTOR3 cameraPosition, D3DXVECTOR3 BillboardPosition)
{
	float angle;
	float rotation;
	angle = atan2(BillboardPosition.x - cameraPosition.x, BillboardPosition.z - cameraPosition.z) * (180.0 / D3DX_PI);
	rotation = (float)angle * 0.0174532925f;

	return rotation;
}