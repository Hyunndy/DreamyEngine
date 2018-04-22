////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"




GraphicsClass::GraphicsClass()
{
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
	D3DInfo info;
	D3D::GetInfo(&info);
	D3D::Get();

	// Shader객체 생성
	//-------------------------------------------------------------------------------------
	m_Shader = new ShaderManagerClass;
	if (!m_Shader) { return false; }
	
	result = m_Shader->Initialize(hwnd);
	if (!result){MessageBox(hwnd, L"Could not initialize  shader object.", L"Error", MB_OK); return false;}
	//-------------------------------------------------------------------------------------

	// Camera객체 생성
	//-------------------------------------------------------------------------------------
	Camera::Get()->SetPosition(D3DXVECTOR3(0.0f, 0.0f, -1.0f));
	Camera::Get()->GetView(&ImageViewMatrix);
	Camera::Get()->SetPosition(D3DXVECTOR3(0.0f, 3.0f, -10.0f));
	Camera::Get()->GetView(&cupViewMatrix);
	Camera::Delete();
	//-------------------------------------------------------------------------------------

	// 메인이미지 생성
	//-------------------------------------------------------------------------------------
	m_Start = new ImageClass;
	if (!m_Start) { return false; }

	result = m_Start->Initialize(screenWidth, screenHeight, L"../Dreamy/Data/Main.jpg", screenWidth, screenHeight);
	if (!result) { MessageBox(hwnd, L"Could not initialize the main object.", L"Error", MB_OK); return false; }
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

	// 로딩이미지 객체 생성 및 출력
	//-------------------------------------------------------------------------------------
	SAFE_SHUTDOWN(m_Start);

	m_Loading = new ImageClass;

	result = m_Loading->Initialize( screenWidth, screenHeight, L"../Dreamy/Data/Loading.jpg", screenWidth, screenHeight);
	if (!result) { MessageBox(hwnd, L"Could not initialize model object", L"Error", MB_OK); return false; }

	RenderLoadingScene();
	//-------------------------------------------------------------------------------------



	// FBXAnimation
	//--------------------------------------------------------------------------------------
	FBXShader::CreateBuffer();

	wstring filePath = L"../Dreamy/Contents/Models/Sword and Shield Pack/";

	//model = new ModelScene(L"./Contents/Models/Box.fbx");
	//model = new ModelScene(L"./Contents/Models/Rock_01.fbx");

	tPosePath = filePath + L"Horse.fbx";
	m_horse->TexturePath = filePath + L"Horse_D.png";
	idlePath = filePath + L"Horse_Idle.fbx";
	runPath = filePath + L"Horse_Run.fbx";
	walkPath = filePath + L"Horse_Walk.fbx";

	m_horse = new ModelScene();

	m_horse->LoadScene(tPosePath, true, true, true, false);
	//m_horse->LoadScene(runPath, false, false, false, true);

	// Default Animation 설정
	//m_horse->SetCurrentAnimation(runPath);

	D3DXMATRIX HorseWorldMatrix;
	D3DXMatrixIdentity(&HorseWorldMatrix);
	D3DXMatrixTranslation(&HorseWorldMatrix, 800.0f, 20.0f, 462.0f);

	m_horse->SetWorldTransform(HorseWorldMatrix);

	m_npc = new ModelScene();
	
	wstring tPosePath2 = filePath + L"Elf.fbx";
	m_npc->TexturePath = filePath + L"Elf_D.png";
	wstring idlePath2 = filePath + L"Elf_Idle.fbx";

	m_npc->LoadScene(tPosePath2, true, true, true, false);
	m_npc->LoadScene(idlePath2, false, false, false, true);

	// Default Animation 설정
	m_npc->SetCurrentAnimation(idlePath2);

	D3DXMATRIX NpcWorldMatrix;
	D3DXMATRIX NpcTranslationMatrix;
	D3DXMATRIX NpcRotationMatrix;

	D3DXMatrixIdentity(&NpcWorldMatrix);
	D3DXMatrixIdentity(&NpcRotationMatrix);
	D3DXMatrixIdentity(&NpcTranslationMatrix);
	D3DXMatrixRotationY(&NpcRotationMatrix, -35.0f);
	D3DXMatrixTranslation(&NpcTranslationMatrix, 545.0f, 10.0f, 345.0f);
	D3DXMatrixMultiply(&NpcWorldMatrix, &NpcRotationMatrix, &NpcTranslationMatrix);

	

	m_npc->SetWorldTransform(NpcWorldMatrix);
	//--------------------------------------------------------------------------------------

	// OBJ모델
	//--------------------------------------------------------------------------------------
	m_cube = new ModelClass;
	if (!m_cube) { return false; }

	result = m_cube->InitializeSpecMap("../Dreamy/Data/cube.txt", L"../Dreamy/Data/stone02.dds", L"../Dreamy/Data/bump02.dds", L"../Dreamy/Data/spec02.dds");
	if (!result) { MessageBox(hwnd, L"Could not m_cube object", L"Error", MB_OK); return false; }



	m_House = new ModelClass;

	result = m_House->Initialize("../Dreamy/Data/BakerHouse.txt", L"../Dreamy/Data/BakerHouse.png");
	//result = m_House->Initialize("../Dreamy/Data/House2.txt", L"../Dreamy/Data/BakerHouse.png");
	if (!result) { MessageBox(hwnd, L"Could not initialize tree object", L"Error", MB_OK); return false; }
	//--------------------------------------------------------------------------------------

	// Light
	//--------------------------------------------------------------------------------------
	m_Light = new LightClass;
	if (!m_Light) { return false; }

	m_Light->SetAmbientColor(0.05f, 0.05f, 0.05f, 1.0f);
	//m_Light->SetAmbientColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.5f, -1.0f, -0.25f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(16.0f); // 반사강도값이 낮을수록 반사광 효과가 커진다!
	//--------------------------------------------------------------------------------------

	// Frustum
	//--------------------------------------------------------------------------------------
	m_Frustum = new FrustumClass;
	if (!m_Frustum) { return false; }
	//--------------------------------------------------------------------------------------

	// LandScape
	//--------------------------------------------------------------------------------------
	m_Terrain = new TerrainClass;
	if (!m_Terrain) { return false; }

	result = m_Terrain->Initialize("../Dreamy/Terrain/setup.txt", "../Dreamy/Terrain/dirt01d.tga", "../Dreamy/Terrain/dirt01n.tga");
	if (!result) { MessageBox(hwnd, L"Could not initialize Terrain object", L"Error", MB_OK); return false; }

	m_TerrainShader = new TerrainShaderClass;
	if (!m_TerrainShader) { return false; }

	m_TerrainShader->Initialize(hwnd);
	if (!result) { MessageBox(hwnd, L"Could not initialize TerrainShader object", L"Error", MB_OK); return false; }

	m_Sky = new SkyClass;
	if (!m_Sky) { return false; }

	result = m_Sky->InitializeSky();
	if (!result) { MessageBox(hwnd, L"Could not initialize Sky object", L"Error", MB_OK); return false; }

	result = m_Sky->InitializeCloud( L"../Dreamy/Data/cloud001.dds", L"../Dreamy/Data/cloud002.dds");
	if (!result) { MessageBox(hwnd, L"Could not initialize Cloud object", L"Error", MB_OK); return false; }

	//--------------------------------------------------------------------------------------


	// 호수
	//-------------------------------------------------------------------------------------
	// Create the refraction render to texture object.
	m_RefractionTexture = new RTTTextureClass;
	if (!m_RefractionTexture) { return false; }

	// Initialize the refraction render to texture object.
	result = m_RefractionTexture->Initialize( screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result) { MessageBox(hwnd, L"Could not initialize the refraction render to texture object.", L"Error", MB_OK); return false; }

	// Create the water object.
	m_Water = new WaterClass;
	if (!m_Water) { return false; }

	// Initialize the water object.
	result = m_Water->Initialize( L"../Dreamy/Data/waternormal.dds", 11.0f, 55.0f);
	if (!result) { MessageBox(hwnd, L"Could not initialize the water object.", L"Error", MB_OK); return false; }

	m_WaterTerrain = new TerrainClass;
	if (!m_WaterTerrain) { return false; }

	result = m_WaterTerrain->Initializeforwater("../Dreamy/Terrain/setup.txt", "../Dreamy/Terrain/dirt01d.tga", "../Dreamy/Terrain/dirt01n.tga");
	if (!result) { MessageBox(hwnd, L"Could not initialize m_WaterTerrain object", L"Error", MB_OK); return false; }


	m_WaterTerrainShader = new TerrainShaderClass;
	if (!m_WaterTerrainShader) { return false; }

	m_WaterTerrainShader->Initialize( hwnd);
	if (!result) { MessageBox(hwnd, L"Could not initialize m_WaterTerrainShader object", L"Error", MB_OK); return false; }
	//-------------------------------------------------------------------------------------

	// Text
	//--------------------------------------------------------------------------------------
	m_Text = new TextClass;
	if (!m_Text) { return false; }

	result = m_Text->Initialize(hwnd, screenWidth, screenHeight, ImageViewMatrix);
	if (!result) { MessageBox(hwnd, L"m_Text error", L"Error", MB_OK); return false; }
	//--------------------------------------------------------------------------------------

	// 2D이미지
	//--------------------------------------------------------------------------------------
	m_CrossHair = new ImageClass;
	if (!m_CrossHair) { return false; }
	
    result = m_CrossHair->Initialize(screenWidth, screenHeight, L"../Dreamy/Data/sibal.png", screenWidth, screenHeight);
	if (!result) { MessageBox(hwnd, L"Could not initialize Crosshair", L"Error", MB_OK); return false; }

	m_MouseCursor = new ImageClass;
	if (!m_MouseCursor) { return false; }

	result = m_MouseCursor->Initialize(screenWidth, screenHeight, L"../Dreamy/Data/mouse.dds", 32, 32);
	if (!result) { MessageBox(hwnd, L"m_MouseCursor error", L"Error", MB_OK); return false; }

	m_UI = new ImageClass;
	
	result = m_UI->Initialize(screenWidth, screenHeight, L"../Dreamy/Data/UI.png", screenWidth, screenHeight);
	if (!result) { MessageBox(hwnd, L"UI error", L"Error", MB_OK); return false; }

	//--------------------------------------------------------------------------------------

	// 인스턴싱 객체들 생성
	//--------------------------------------------------------------------------------------
	m_Instancing = new InstancingClass;
	if (!m_Instancing) { return false; }

	m_Instancing->SetInstanceCount(6);
	m_Instancing->SetInsatanceVariable(-5.0f, 0.0f, 25.0f);
	m_Instancing->SetInstancePosition(140.0f, 11.0f, 470.0f);

	result = m_Instancing->Initialize( "../Dreamy/Data/MapleTree.txt", L"../Dreamy/Data/bark_0021.jpg");
	if (!result) { MessageBox(hwnd, L"instancing", L"Error", MB_OK); return false; }

	m_Instancing2 = new InstancingClass;
	if (!m_Instancing2) { return false; }
	//124 340
	m_Instancing2->SetInstanceCount(6);
	m_Instancing2->SetInsatanceVariable(-5.0f, 0.0f, 25.0f);
	m_Instancing2->SetInstancePosition(140.0f, 11.0f, 470.0f);

	result = m_Instancing2->Initialize("../Dreamy/Data/MapleTreeLeaves.txt", L"../Dreamy/Data/green_leaf.png");
	if (!result) { MessageBox(hwnd, L"instancing", L"Error", MB_OK); return false; }

	m_Instancing3 = new InstancingClass;
	if (!m_Instancing3) { return false; }

	m_Instancing3->SetInstanceCount(7);
	m_Instancing3->SetInsatanceVariable(-25.0f, 0.0f, 0.0f);
	m_Instancing3->SetInstancePosition(285.0f, 16.0f, 350.0f);

	result = m_Instancing3->Initialize("../Dreamy/Data/MapleTree.txt", L"../Dreamy/Data/bark_0021.jpg");
	if (!result) { MessageBox(hwnd, L"instancing", L"Error", MB_OK); return false; }

	m_Instancing4 = new InstancingClass;
	if (!m_Instancing4) { return false; }
	//124 340
	m_Instancing4->SetInstanceCount(7);
	m_Instancing4->SetInsatanceVariable(-25.0f, 0.0f, 0.0f);
	m_Instancing4->SetInstancePosition(285.0f, 16.0f, 350.0f);

	result = m_Instancing4->Initialize("../Dreamy/Data/MapleTreeLeaves.txt", L"../Dreamy/Data/green_leaf.png");
	if (!result) { MessageBox(hwnd, L"instancing", L"Error", MB_OK); return false; }

	m_Instancing5 = new InstancingClass;
	if (!m_Instancing5) { return false; }
	//124 340
	m_Instancing5->SetInstanceCount(7);
	m_Instancing5->SetInsatanceVariable(-25.0f, 0.0f, 0.0f);
	m_Instancing5->SetInstancePosition(285.0f, 10.0f, 600.0f);

	result = m_Instancing5->Initialize("../Dreamy/Data/MapleTree.txt", L"../Dreamy/Data/bark_0021.jpg");
	if (!result) { MessageBox(hwnd, L"instancing", L"Error", MB_OK); return false; }

	m_Instancing6 = new InstancingClass;
	if (!m_Instancing6) { return false; }
	//124 340
	m_Instancing6->SetInstanceCount(7);
	m_Instancing6->SetInsatanceVariable(-25.0f, 0.0f, 0.0f);
	m_Instancing6->SetInstancePosition(285.0f, 10.0f, 600.0f);

	result = m_Instancing6->Initialize("../Dreamy/Data/MapleTreeLeaves.txt", L"../Dreamy/Data/green_leaf.png");
	if (!result) { MessageBox(hwnd, L"instancing", L"Error", MB_OK); return false; }

	//744 51 554 x축커지게
	m_Instancing7 = new InstancingClass;
	if (!m_Instancing7) { return false; }

	m_Instancing7->SetInstanceCount(5);
	m_Instancing7->SetInsatanceVariable(25.0f, 0.0f, 0.0f);
	m_Instancing7->SetInstancePosition(735.0f, 13.0f, 530.0f);

	result = m_Instancing7->Initialize("../Dreamy/Data/MapleTree.txt", L"../Dreamy/Data/bark_0021.jpg");
	if (!result) { MessageBox(hwnd, L"instancing", L"Error", MB_OK); return false; }

	m_Instancing8 = new InstancingClass;
	if (!m_Instancing8) { return false; }

	m_Instancing8->SetInstanceCount(5);
	m_Instancing8->SetInsatanceVariable(25.0f, 0.0f, 0.0f);
	m_Instancing8->SetInstancePosition(735.0f, 13.0f, 530.0f);

	result = m_Instancing8->Initialize("../Dreamy/Data/MapleTreeLeaves.txt", L"../Dreamy/Data/maple_leaf.png");
	if (!result) { MessageBox(hwnd, L"instancing", L"Error", MB_OK); return false; }

	m_Instancing9 = new InstancingClass;
	if (!m_Instancing9) { return false; }

	m_Instancing9->SetInstanceCount(9);
	m_Instancing9->SetInsatanceVariable(20.0f, 0.0f, 0.0f);
	m_Instancing9->SetInstancePosition(722.0f, 13.0f, 548.0f);

	result = m_Instancing9->Initialize("../Dreamy/Data/MapleTree.txt", L"../Dreamy/Data/bark_0021.jpg");
	if (!result) { MessageBox(hwnd, L"instancing", L"Error", MB_OK); return false; }


	m_Instancing10 = new InstancingClass;
	if (!m_Instancing10) { return false; }

	m_Instancing10->SetInstanceCount(9);
	m_Instancing10->SetInsatanceVariable(20.0f, 0.0f, 0.0f);
	m_Instancing10->SetInstancePosition(722.0f, 13.0f, 548.0f);

	result = m_Instancing10->Initialize("../Dreamy/Data/MapleTreeLeaves.txt", L"../Dreamy/Data/maple_leaf.png");
	if (!result) { MessageBox(hwnd, L"instancing", L"Error", MB_OK); return false; }

	//--------------------------------------------------------------------------------------

	// 미니맵
	//--------------------------------------------------------------------------------------
	m_Minimap = new MinimapClass;
	if (!m_Minimap) { return false; }

	result = m_Minimap->Initialize( screenWidth, screenHeight, 1025, 1025);
	if (!result) { MessageBox(hwnd, L"minimap error", L"Error", MB_OK); return false; }
	//--------------------------------------------------------------------------------------

	// 빌보딩 객체(파티클, 나무 등)
	//--------------------------------------------------------------------------------------
	m_Particle = new ParticleSystem;
	if (!m_Particle) { return false; }

	result = m_Particle->Initialize( L"../Dreamy/Data/star.dds");
	if (!result) { MessageBox(hwnd, L"Could not particle TitleText", L"Error", MB_OK); return false; }

	m_Tree = new ModelClass;

	result = m_Tree->Initialize("../Dreamy/Data/square2.txt", L"../Dreamy/Data/Billboard.png");
	if (!result) { MessageBox(hwnd, L"Could not initialize tree object", L"Error", MB_OK); return false; }

	//--------------------------------------------------------------------------------------

	return true;
}

// 모든 그래픽 객체의 종료가 여기에서 일어난다.
void GraphicsClass::Shutdown()
{

	FBXShader::DeleteBuffer();

	SAFE_SHUTDOWN(m_UI);
	SAFE_SHUTDOWN(m_Minimap);
	SAFE_DELETE(m_horse);
	SAFE_DELETE(m_npc);
	SAFE_SHUTDOWN(m_Tree);
	SAFE_DELETE(m_Light);
	SAFE_SHUTDOWN(m_TerrainShader);
	SAFE_SHUTDOWN(m_Water);
	SAFE_SHUTDOWN(m_WaterTerrain);
	SAFE_SHUTDOWN(m_WaterTerrainShader);
	SAFE_SHUTDOWN(m_RefractionTexture);
	SAFE_SHUTDOWN(m_Instancing);
	SAFE_SHUTDOWN(m_Instancing2);
	SAFE_SHUTDOWN(m_Instancing3);
	SAFE_SHUTDOWN(m_Instancing4);
	SAFE_SHUTDOWN(m_Instancing5);
	SAFE_SHUTDOWN(m_Instancing6);
	SAFE_SHUTDOWN(m_Instancing7);
	SAFE_SHUTDOWN(m_Instancing8);
	SAFE_SHUTDOWN(m_Instancing9);
	SAFE_SHUTDOWN(m_Instancing10);
	SAFE_SHUTDOWN(m_Terrain);
	SAFE_SHUTDOWN(m_House);
	SAFE_SHUTDOWN(m_Sky);
	SAFE_SHUTDOWN(m_cube);
	SAFE_SHUTDOWN(m_Loading);
	SAFE_SHUTDOWN(m_CrossHair);
	SAFE_DELETE(m_Frustum);
	SAFE_SHUTDOWN(m_MouseCursor);
	SAFE_SHUTDOWN(m_Text);
	SAFE_SHUTDOWN(m_Particle);
	Camera::Delete();
	Rasterizer::Delete();
	Blender::Delete();
	DepthStencil::Delete();
	D3D::Delete();
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
	bool result;


	// Model들 Frame처리
	//--------------------------------------------------------------------------------------
	//m_horse->Update(); //말1
	m_npc->Update(); //말2

	m_Sky->Frame(frameTime*0.00001f, 0.0f, frameTime*0.00002f, 0.0f); //구름
	m_Particle->Frame(frameTime); // 파티클
	m_Minimap->PositionUpdate(CameraPos.x, CameraPos.z); //미니맵
	m_Water->Frame(frameTime);
	//--------------------------------------------------------------------------------------
	
	// Camera Frame처리
	//--------------------------------------------------------------------------------------
	bool foundHeight;
	float CameraY;
	Camera::Get()->GetPosition(&CameraPos);
	
	foundHeight = m_Terrain->GetHeightAtPosition(CameraPos.x, CameraPos.z, CameraY);
	if (foundHeight)
		CameraPos.y = CameraY + 40.0f;

	Camera::Get()->SetPosition(CameraPos);
	Camera::Get()->Update();
	//--------------------------------------------------------------------------------------

	// 마우스 
	//--------------------------------------------------------------------------------------
	MousePosX = mouseX;
	MousePosY = mouseY;
	//--------------------------------------------------------------------------------------

	// 텍스트 Frame처리
	//--------------------------------------------------------------------------------------
	// fps세팅
	result = m_Text->SetFps(fps);
	if (!result) { return false; }

	// cpu세팅
	result = m_Text->SetCpu(cpu);
	if (!result) { return false; }
	//--------------------------------------------------------------------------------------
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

	result = RenderRefractionToTexture(Pressed);
	if (!result) { return false; }
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
	bool result;


	//안개 객체 초기화
	//fogEnd가 멀어질수록 밝아짐
	//-------------------------------------------------------------------------------------
	float fogColor, fogStart, fogEnd;
	fogColor = 0.0f;
	fogStart = 400.0f;
	fogEnd = 550.0f;
	//-------------------------------------------------------------------------------------

	//기본행렬
	//-------------------------------------------------------------------------------------
	D3DXMatrixIdentity(&worldMatrix);
	Camera::Get()->GetView(&viewMatrix);
	Camera::Get()->GetProjection(&projectionMatrix);
	Camera::Get()->GetOrtho(&orthoMatrix);

	//시작
	//-------------------------------------------------------------------------------------
	D3D::Get()->BeginScene();

	//시야 절두체(Frustum)생성
	//-------------------------------------------------------------------------------------
	m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);
	//-------------------------------------------------------------------------------------

	// Sky
	//-------------------------------------------------------------------------------------
	D3DXMATRIX SkyworldMatrix;
	D3DXMatrixIdentity(&SkyworldMatrix);
	D3DXVECTOR3 cameraPosition;
	Camera::Get()->GetPosition(&cameraPosition);
	D3DXMatrixTranslation(&SkyworldMatrix, cameraPosition.x, cameraPosition.y, cameraPosition.z);

	//컬링모드 끄기
	Rasterizer::Get()->SetOffCullMode();
	//후면버퍼 끄기
	DepthStencil::Get()->SetOffState();

	m_Sky->RenderSky();
	m_Shader->RenderSkydomeShader(m_Sky->GetIndexCount(), SkyworldMatrix, viewMatrix, projectionMatrix,
		m_Sky->GetApexColor(), m_Sky->GetCenterColor());

	//컬링모드켜기
	Rasterizer::Get()->SetOnCullMode();

	//알파블렌딩 혼합 상태 지정
	Blender::Get()->SetAdd();

	m_Sky->RenderCloud();

	m_Shader->RenderCloudShader(m_Sky->GetCloudIndexCount(), SkyworldMatrix, viewMatrix, projectionMatrix,
		m_Sky->GetCloudTexture1(), m_Sky->GetCloudTexture2(), m_Sky->GetTranslation(0), m_Sky->GetTranslation(1),
		m_Sky->GetTranslation(2), m_Sky->GetTranslation(3), m_Sky->GetBrightness());

	//알파블렌딩 원래대로
	Blender::Get()->SetOff();
	//후면버퍼 키기
	DepthStencil::Get()->SetOnState();
	//-------------------------------------------------------------------------------------

	// Terrain
	//-------------------------------------------------------------------------------------
	// Terrain에서 정한 셀 갯수만큼 셀을 로드한다.
	for (int i = 0; i < m_Terrain->GetCellCount(); i++)
	{
		// Put the terrain cell buffers on the pipeline.
		result = m_Terrain->RenderCell(i, m_Frustum);
		if (result)
		{

			result = m_TerrainShader->Render(m_Terrain->GetCellIndexCount(i), worldMatrix, viewMatrix,
				projectionMatrix, m_Terrain->GetColorTexture(), m_Terrain->GetNormalMapTexture(), m_Light->GetDirection(), m_Light->GetDiffuseColor(), fogStart, fogEnd);
			if (!result) { return false; }

			//m_Terrain->RenderCellLines(m_D3D->GetDeviceContext(), i);
			//m_Shader->RenderColorShader(m_D3D->GetDeviceContext(), m_Terrain->GetCellLinesIndexCount(i), TerrainworldMatrix, viewMatrix, projectionMatrix);
			//if (!result) { return false; }
		}
	}
	//-------------------------------------------------------------------------------------

	// 호수  512.5f, 30.0f, 310.0f
	//-------------------------------------------------------------------------------------
	D3DXMATRIX WaterWorldMatrix, WaterRotationMatrix, WaterreflectionViewMatrix;
	Camera::Get()->RenderWaterReflection(m_Water->GetWaterHeight());
	WaterreflectionViewMatrix = Camera::Get()->GetReflectionViewMatrix();

	D3DXMatrixTranslation(&WaterWorldMatrix, 766.0f, m_Water->GetWaterHeight(), 470.0f);
	D3DXMatrixRotationY(&WaterRotationMatrix, 70.0f);
	D3DXMatrixMultiply(&WaterWorldMatrix, &WaterRotationMatrix, &WaterWorldMatrix);
	m_Water->Render();

	result = m_Shader->RenderWaterShader(m_Water->GetIndexCount(), WaterWorldMatrix, viewMatrix, projectionMatrix, WaterreflectionViewMatrix,
		m_RefractionTexture->GetShaderResourceView(), m_Water->GetTexture(),
		CameraPos, m_Water->GetNormalMapTiling(), m_Water->GetWaterTranslation(), m_Water->GetReflectRefractScale(),
		m_Water->GetRefractionTint(), m_Light->GetDirection(), m_Water->GetSpecularShininess());
	if (!result) { return false; }
	//-------------------------------------------------------------------------------------

	// FBXANIMATIONModel들
	//-------------------------------------------------------------------------------------
	//m_horse->Render();
	m_npc->Render();

	// OBJ모델
	//-------------------------------------------------------------------------------------

	// 스페큘러맵 박스
	//-----------------------------
	static float textureTranslation = 0.0f;
	textureTranslation += 0.003f;
	if (textureTranslation > 1.0f) { textureTranslation -= 1.0f; }

	D3DXMATRIX Cube3WorldMatrix;
	D3DXMatrixIdentity(&Cube3WorldMatrix);

	if (sibal == false)
	{
		m_cube->Translation(767.0f, 26.0f, 389.0f);
		m_cube->RotationY(-40.0f);
		m_cube->Scale(7.0f, 7.0f, 7.0f);
		m_cube->Multiply(m_cube->GetRotationYMatrix(), m_cube->GetTranslationMatrix());
		m_cube->Multiply(m_cube->GetScailingMatrix(), m_cube->GetFinalMatrix());
		D3DXMatrixMultiply(&Cube3WorldMatrix, &m_cube->GetFinalMatrix(), &Cube3WorldMatrix);
	}
	else
	{
		D3DXMatrixIdentity(&Cube3WorldMatrix);
		//m_cube->Translation(CameraPos.x, CameraPos.y - 10.0f, CameraPos.z + 20.0f);
		//m_cube->RotationX(CalculateBillboarding(CameraPos, D3DXVECTOR3(CameraPos.x, CameraPos.y - 10.0f, CameraPos.z + 20.0f)));
	}



	m_cube->Render();

	if (sibal == false)
	{
		result = m_Shader->RenderTranslateShader(m_cube->GetIndexCount(), Cube3WorldMatrix, viewMatrix, projectionMatrix
			, m_cube->GetTripleTextureArray(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), CameraPos
			, m_Light->GetSpecularColor(), m_Light->GetSpecularPower(), textureTranslation);
	}
	else
		result = m_Shader->RenderTranslateShader(m_cube->GetIndexCount(), Cube3WorldMatrix, cupViewMatrix, projectionMatrix
			, m_cube->GetTripleTextureArray(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), CameraPos
			, m_Light->GetSpecularColor(), m_Light->GetSpecularPower(), textureTranslation);

	if (!result) { return false; }

	// 집
	//-----------------------------
	D3DXMATRIX HouseWorldMatrix;
	D3DXMatrixIdentity(&HouseWorldMatrix);

	//D3DXMATRIX Rotation, Scale, Translation;
	//D3DXMatrixRotationY(&Rotation, -30.0f);
	//D3DXMatrixTranslation(&Translation, 70.0f, 10.0f, 700.0f);
	//D3DXMatrixScaling(&Scale, 0.7f, 0.7f, 0.7f);

	m_House->Translation(70.0f, 8.0f, 730.0f);
	m_House->RotationY(-30.0f);
	//m_House->Scale(0.7f, 0.7f, 0.7f);
	//m_House->GetRotationYMatrix());
	m_House->Multiply(m_House->GetRotationYMatrix(), m_House->GetTranslationMatrix());

	//D3DXMatrixMultiply(&HouseWorldMatrix, &HouseWorldMatrix, &Scale);
	//3DXMatrixMultiply(&HouseWorldMatrix, &HouseWorldMatrix, &Rotation);
	D3DXMatrixMultiply(&HouseWorldMatrix, &HouseWorldMatrix, &m_House->GetFinalMatrix());

	m_House->Render();

	result = m_Shader->RenderTextureShader(m_House->GetIndexCount(), HouseWorldMatrix, viewMatrix, projectionMatrix, m_House->GetTexture());
	if (!result) { return false; }

	//-------------------------------------------------------------------------------------

	// 파티클
	//-------------------------------------------------------------------------------------
	D3DXVECTOR3 ParticlePosition = { 767.0f, 30.0f, 389.0f };
	D3DXMATRIX ParticleWorldMatrix;
	D3DXMatrixIdentity(&ParticleWorldMatrix);
	m_Particle->Translation(ParticlePosition.x, ParticlePosition.y, ParticlePosition.z);
	m_Particle->RotationY(CalculateBillboarding(CameraPos, ParticlePosition));
	m_Particle->Multiply(m_Particle->GetRotationYMatrix(), m_Particle->GetTranslationMatrix());
	D3DXMatrixMultiply(&ParticleWorldMatrix, &m_Particle->GetFinalMatrix(), &ParticleWorldMatrix);

	if (m_Particle->active == true)
	{
		Blender::Get()->SetAdd();

		m_Particle->Render();

		result = m_Shader->RenderParticleShader( m_Particle->GetIndexCount(), ParticleWorldMatrix, viewMatrix, projectionMatrix, m_Particle->GetTexture());
		if (!result) { return false; }

	}
	//-------------------------------------------------------------------------------------

	// 인스턴싱, 빌보딩객체
	//-------------------------------------------------------------------------------------

	// 인스턴싱
	//-----------------------------
	Rasterizer::Get()->SetOffCullMode();
	Blender::Get()->SetLinear();

	m_Instancing->Render();
	
	result = m_Shader->RenderInstancingShader(m_Instancing->GetVertexCount(), m_Instancing->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing->GetTexture());
	if (!result) { return false; }
	
	m_Instancing2->Render();
	
	result = m_Shader->RenderInstancingShader(m_Instancing2->GetVertexCount(), m_Instancing2->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing2->GetTexture());
	if (!result) { return false; }

	m_Instancing3->Render();

	result = m_Shader->RenderInstancingShader(m_Instancing3->GetVertexCount(), m_Instancing3->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing3->GetTexture());
	if (!result) { return false; }

	m_Instancing4->Render();

	result = m_Shader->RenderInstancingShader(m_Instancing4->GetVertexCount(), m_Instancing4->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing4->GetTexture());
	if (!result) { return false; }

	m_Instancing5->Render();

	result = m_Shader->RenderInstancingShader(m_Instancing5->GetVertexCount(), m_Instancing5->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing5->GetTexture());
	if (!result) { return false; }

	m_Instancing6->Render();

	result = m_Shader->RenderInstancingShader(m_Instancing6->GetVertexCount(), m_Instancing6->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing6->GetTexture());
	if (!result) { return false; }

	m_Instancing7->Render();

	result = m_Shader->RenderInstancingShader(m_Instancing7->GetVertexCount(), m_Instancing7->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing7->GetTexture());
	if (!result) { return false; }

	m_Instancing8->Render();

	result = m_Shader->RenderInstancingShader(m_Instancing8->GetVertexCount(), m_Instancing8->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing8->GetTexture());
	if (!result) { return false; }

	m_Instancing9->Render();

	result = m_Shader->RenderInstancingShader(m_Instancing9->GetVertexCount(), m_Instancing9->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing9->GetTexture());
	if (!result) { return false; }

	m_Instancing10->Render();

	result = m_Shader->RenderInstancingShader(m_Instancing10->GetVertexCount(), m_Instancing10->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing10->GetTexture());
	if (!result) { return false; }

	Rasterizer::Get()->SetOnCullMode();




	// 나무 빌보딩
	//-----------------------------
	D3DXVECTOR3 TreePosition;
	TreePosition = { 480.0f, 105.0f, 537.0f };

	D3DXMATRIX TreeWorldMatrix;
	D3DXMatrixIdentity(&TreeWorldMatrix);

	m_Tree->Translation(480.0f, 105.0f, 537.0f);
	m_Tree->RotationY(CalculateBillboarding(CameraPos, TreePosition));
	m_Tree->Scale(1.5f, 1.5f, 1.5f);

	m_Tree->Multiply(m_Tree->GetScailingMatrix(), m_Tree->GetRotationYMatrix());
	m_Tree->Multiply(m_Tree->GetFinalMatrix(), m_Tree->GetTranslationMatrix());
	D3DXMatrixMultiply(&TreeWorldMatrix,  &TreeWorldMatrix, &m_Tree->GetFinalMatrix());

	m_Tree->Render();
	result = m_Shader->RenderTextureShader(m_Tree->GetIndexCount(), TreeWorldMatrix, viewMatrix, projectionMatrix, m_Tree->GetTexture());
	if (!result) { return false; }
	//-----------------------------

	// 2D
	//-------------------------------------------------------------------------------------
	DepthStencil::Get()->SetOffState();
	//Blender::Get()->SetLinear();


	// Text
	//------------------------------------------------
	result = m_Text->SetMousePosition(MousePosX, MousePosY);
	result = m_Text->SetPosition(CameraPos.x, CameraPos.y, CameraPos.z);
	result = m_Text->Render(worldMatrix, orthoMatrix);

	// 이미지들
	//------------------------------------------------
	result = m_Minimap->Render( m_Shader->m_TextureShader, worldMatrix, ImageViewMatrix, orthoMatrix);
	if (!result) { return false; }

	m_CrossHair->Render(0, 0);
	if (!result) { return false; }
	m_Shader->RenderTextureShader(m_CrossHair->GetIndexCount(), worldMatrix, ImageViewMatrix, orthoMatrix, m_CrossHair->GetTexture());
	if (!result) { return false; }

	m_MouseCursor->Render(MousePosX, MousePosY);
	m_MouseCursor->Scale(1.5f, 1.5f,1.5f);

	D3DXMatrixMultiply(&m_MouseCursor->FinalMatrix, &worldMatrix, &m_MouseCursor->ScaleMatrix);
	m_Shader->RenderTextureShader(m_MouseCursor->GetIndexCount(), m_MouseCursor->FinalMatrix, ImageViewMatrix, orthoMatrix, m_MouseCursor->GetTexture());


	if (m_UI->active == true)
	{
		m_UI->Render(0, 0);
		if (!result) { return false; }
		m_Shader->RenderTextureShader(m_UI->GetIndexCount(), worldMatrix, ImageViewMatrix, orthoMatrix, m_UI->GetTexture());
		if (!result) { return false; }
	}




	Blender::Get()->SetOff();
	DepthStencil::Get()->SetOnState();
	//-------------------------------------------------------------------------------------



	//버퍼에 그려진 씬을 화면에 표시한다.
	D3D::Get()->EndScene();
	



	return true;
}

//-------------------------------------------------------------------------------------
//이름: RenderRefractionToTexture()
//용도: 호수 내부의 모습을 RTT로 굴절(Refraction)한다.
//-------------------------------------------------------------------------------------
bool GraphicsClass::RenderRefractionToTexture(bool Pressed)
{
	D3DXVECTOR4 clipPlane;

	bool result;

	// Setup a clipping plane based on the height of the water to clip everything above it to create a refraction.
	clipPlane = D3DXVECTOR4(0.0f, -1.0f, 0.0f, m_Water->GetWaterHeight() + 0.1f);

	// Set the render target to be the refraction render to texture.
	m_RefractionTexture->SetRenderTarget();

	// Clear the refraction render to texture.
	m_RefractionTexture->ClearRenderTarget( 0.0f, 0.0f, 0.0f, 1.0f);


	m_WaterTerrain->Renderforwater();

	result = m_Shader->RenderWaterReflectionShader( m_WaterTerrain->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_WaterTerrain->GetColorTexture(), m_WaterTerrain->GetNormalMapTexture(), m_Light->GetDiffuseColor(),
		m_Light->GetDirection(), 2.0f, clipPlane);
	if (!result) { return false; }

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	DepthStencil::Get()->SetDefaultRenderView();

	// Reset the viewport back to the original.
	Camera::Get()->ResetViewPort();

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
	Camera::Get()->GetOrtho(&orthoMatrix);
	D3DXMatrixIdentity(&MainSceneworldMatrix);

	D3D::Get()->BeginScene();

	result = m_Loading->Render(0, 0);
	if (!result) { return false; }

	result = m_Shader->RenderTextureShader(m_Loading->GetIndexCount(),MainSceneworldMatrix, ImageViewMatrix, orthoMatrix, m_Loading->GetTexture());
	if (!result) { return false; }


	D3D::Get()->EndScene();

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
	Camera::Get()->GetOrtho(&orthoMatrix);
	D3DXMatrixIdentity(&MainSceneworldMatrix);

	D3D::Get()->BeginScene();

	result = m_Start->Render(0, 0);
	if (!result) { return false; }

	result = m_Shader->RenderTextureShader(m_Start->GetIndexCount(), MainSceneworldMatrix, ImageViewMatrix, orthoMatrix, m_Start->GetTexture());
	if (!result) { return false; }


	D3D::Get()->EndScene();

	return true;
}

void GraphicsClass::CheckIntersection(int mouseX, int mouseY, int m_screenWidth, int m_screenHeight)
{
	//D3DXMATRIX ProjectionMatrix, ViewMatrix, WorldMatrix;
	//D3DXMatrixIdentity(&WorldMatrix);
	//Camera::Get()->GetView(&ViewMatrix);
	//Camera::Get()->GetProjection(&ProjectionMatrix);


	//sibal = m_cube->TestIntersection(mouseX, mouseY, m_screenWidth, m_screenHeight, projectionMatrix, viewMatrix, worldMatrix, CameraPos);

		if (m_cube->TestIntersection(mouseX, mouseY, m_screenWidth, m_screenHeight, projectionMatrix, viewMatrix, worldMatrix, CameraPos))
		{
			sibal = true;
			m_Particle->active = true;
			m_UI->active = true;
		}

		else
		{
			m_UI->active = false;
		}

}

float GraphicsClass::CalculateBillboarding(D3DXVECTOR3 cameraPosition, D3DXVECTOR3 BillboardPosition)
{
	float angle;
	float rotation;
	angle = atan2(BillboardPosition.x - cameraPosition.x, BillboardPosition.z - cameraPosition.z) * (180.0 / D3DX_PI);
	rotation = (float)angle * 0.0174532925f;

	return rotation;
}