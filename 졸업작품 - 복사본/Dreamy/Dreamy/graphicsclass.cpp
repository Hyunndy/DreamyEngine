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

	width = info.screenWidth;
	height = info.screenHeight;
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
	Camera::Delete();
	Camera::Get()->SetPosition(D3DXVECTOR3(0.0f, 3.0f, -10.0f));
	Camera::Get()->GetView(&BalloonViewMatrix);
	Camera::Delete();
	//-------------------------------------------------------------------------------------

	// 메인이미지 생성
	//-------------------------------------------------------------------------------------
	m_Start = new ImageClass;
	if (!m_Start) { return false; }

	result = m_Start->Initialize(screenWidth, screenHeight, L"../Dreamy/Data/Main.jpg", screenWidth, screenHeight);
	if (!result) { MessageBox(hwnd, L"Could not initialize the main object.", L"Error", MB_OK); return false; }
	//-------------------------------------------------------------------------------------

	// 마우스 커서 생성
	//-------------------------------------------------------------------------------------
	m_MouseCursor = new ImageClass;
	if (!m_MouseCursor) { return false; }

	result = m_MouseCursor->Initialize(screenWidth, screenHeight, L"../Dreamy/Data/mouse.dds", 32, 32);
	if (!result) { MessageBox(hwnd, L"m_MouseCursor error", L"Error", MB_OK); return false; }

	m_MouseCursor->Scale(1.5f, 1.5f, 1.5f);
	//-------------------------------------------------------------------------------------

	return true;
}



/*----------------------------------------------------------------------------------------
이름 : Loading()
용도 : 실제로 렌더링 되는 애들 초기화
------------------------------------------------------------------------------------------*/
bool GraphicsClass::FBXLoading()
{
	// FBXAnimation
	//--------------------------------------------------------------------------------------
	FBXShader::CreateBuffer();

	wstring filePath = L"../Dreamy/Contents/Models/Sword and Shield Pack/";

	wstring tPosePath = filePath + L"Horse.fbx";
	wstring idlePath = filePath + L"Horse_Idle.fbx";
	wstring runPath = filePath + L"Horse_Run.fbx";


	//말1
	m_horse = new ModelScene();

	m_horse->TexturePath = filePath + L"Horse_D.dds";
	m_horse->LoadScene(tPosePath, true, true, true, false);
	m_horse->LoadScene(runPath, false, false, false, true);

	// Default Animation 설정
	m_horse->SetCurrentAnimation(runPath);
	m_horse->Scale(1.8f, 1.8f, 1.8f);


	//말2
	m_horse2 = new ModelScene();

	m_horse2->TexturePath = m_horse->TexturePath;
	m_horse2->LoadScene(tPosePath, true, true, true, false);
	m_horse2->LoadScene(runPath, false, false, false, true);

	m_horse2->SetCurrentAnimation(runPath);

	m_horse2->Scale(1.8f, 1.8f, 1.8f);

	//말3
	m_horse3 = new ModelScene();

	m_horse3->TexturePath = m_horse->TexturePath;
	m_horse3->LoadScene(tPosePath, true, true, true, false);
	m_horse3->LoadScene(runPath, false, false, false, true);

	m_horse3->SetCurrentAnimation(runPath);

	m_horse3->Scale(1.8f, 1.8f, 1.8f);

	//npc
	m_npc = new ModelScene();

	m_npc->TexturePath = m_horse->TexturePath;
	m_npc->LoadScene(tPosePath, true, true, true, false);
	m_npc->LoadScene(idlePath, false, false, false, true);

	m_npc->SetCurrentAnimation(idlePath);

	m_npc->Scale(1.8f, 1.8f, 1.8f);
	m_npc->RotationY(-35.0f);
	m_npc->Translation(515.0f, 22.0f, 415.0f);

	m_npc->Multiply(m_npc->GetScailingMatrix(), m_npc->GetRotationYMatrix());
	m_npc->Multiply(m_npc->GetFinalMatrix(), m_npc->GetTranslationMatrix());
	m_npc->SetWorldTransform(m_npc->GetFinalMatrix());

	//--------------------------------------------------------------------------------------

	return true;
}
bool GraphicsClass::OBJLoading(HWND hwnd)
{
	bool result;
	// OBJ모델
	//--------------------------------------------------------------------------------------

	//큐브(현재 npc에게 붙어있음)
	m_NpcBox = new ModelClass;
	if (!m_NpcBox) { return false; }

	result = m_NpcBox->Initialize("../Dreamy/Data/cube.txt", L"../Dreamy/Data/point.png");
	if (!result) { MessageBox(hwnd, L"Could not m_NpcBox object", L"Error", MB_OK); return false; }

	m_NpcBox->Translation(513.0f, 30.0f, 410.0f);
	//(513.0f, 55.0f, 405.0f
	m_NpcBox->Scale(1.5f, 1.5f, 1.5f);

	m_NpcBox->Multiply(m_NpcBox->GetScailingMatrix(), m_NpcBox->GetTranslationMatrix());

	//호수 충돌
	m_WaterBox = new ModelClass;
	if (!m_WaterBox) { return false; }

	result = m_WaterBox->Initialize("../Dreamy/Data/cube.txt", L"../Dreamy/Data/point.png");
	if (!result) { MessageBox(hwnd, L"Could not m_NpcBox object", L"Error", MB_OK); return false; }

	m_WaterBox->Translation(766.0f, 11.0f, 470.0f);
	m_WaterBox->Scale(10.0f, 1.0f, 10.0f);

	m_WaterBox->Multiply(m_WaterBox->GetScailingMatrix(), m_WaterBox->GetTranslationMatrix());

	//물풍선
	m_circle = new ModelClass;
	if (!m_circle) { return false; }

	result = m_circle->Initialize("../Dreamy/Data/sphere.txt", L"../Dreamy/Data/water.dds");
	if (!result) { MessageBox(hwnd, L"Could not m_NpcBox object", L"Error", MB_OK); return false; }

	m_circle->Translation(685.0f, 25.0f, 415.0f);
	m_circle->Scale(3.0f, 3.0f, 3.0f);
	m_circle->Multiply(m_circle->GetScailingMatrix(), m_circle->GetTranslationMatrix());

	D3DXMatrixIdentity(&BalloonWorldMatrix);

	//집
	m_House = new ModelClass;

	result = m_House->Initialize("../Dreamy/Data/BakerHouse.txt", L"../Dreamy/Data/BakerHouse.dds");
	//result = m_House->Initialize("../Dreamy/Data/House2.txt", L"../Dreamy/Data/BakerHouse.png");
	if (!result) { MessageBox(hwnd, L"Could not initialize tree object", L"Error", MB_OK); return false; }

	m_House->Translation(70.0f, 8.0f, 730.0f);
	m_House->RotationY(-30.0f);
	m_House->Multiply(m_House->GetRotationYMatrix(), m_House->GetTranslationMatrix());

	//Barrel
	m_Barrel = new ModelClass;

	result = m_Barrel->Initialize("../Dreamy/Data/Barrel.txt", L"../Dreamy/Data/BarrelTexture.dds");
	if (!result) { MessageBox(hwnd, L"Could not initialize m_Barrel object", L"Error", MB_OK); return false; }

	m_Barrel->Translation(260.0f, 21.0f, 570.0f);
	m_Barrel->Scale(0.2f, 0.2f, 0.2f);
	m_Barrel->Multiply(m_Barrel->GetScailingMatrix(), m_Barrel->GetTranslationMatrix());

	m_Barrel2 = new ModelClass;
	if (!m_Barrel2) { return false; }

	result = m_Barrel2->Initialize("../Dreamy/Data/Barrel.txt", L"../Dreamy/Data/BarrelTexture.dds");
	if (!result) { MessageBox(hwnd, L"m_Barrel2", L"Error", MB_OK); return false; }

	//m_Barrel2->Translation(279.5f, 19.5f, 552.0f);
	m_Barrel2->Translation(246.0f, 18.5f, 568.5f);
	m_Barrel2->Scale(0.15f, 0.15f, 0.15f);
	m_Barrel2->Multiply(m_Barrel2->GetScailingMatrix(), m_Barrel2->GetTranslationMatrix());

	m_Barrel3 = new ModelClass;
	if (!m_Barrel3) { return false; }

	result = m_Barrel3->Initialize("../Dreamy/Data/Barrel.txt", L"../Dreamy/Data/BarrelTexture.dds");
	if (!result) { MessageBox(hwnd, L"m_Barrel2", L"Error", MB_OK); return false; }

	//m_Barrel3->Translation(280.0f, 17.5f, 539.0f);
	m_Barrel3->Translation(236.0f, 16.5f, 568.0f);
	m_Barrel3->Scale(0.1f, 0.1f, 0.1f);
	m_Barrel3->Multiply(m_Barrel3->GetScailingMatrix(), m_Barrel3->GetTranslationMatrix());

	// Crate
	m_Crate = new ModelClass;
	if (!m_Crate) { return false; }

	result = m_Crate->Initialize("../Dreamy/Data/Crate.txt", L"../Dreamy/Data/CrateTexture.dds");
	if (!result) { MessageBox(hwnd, L"m_Crate", L"Error", MB_OK); return false; }

	m_Crate->Translation(235.0f, 18.0f, 450.0f);
	m_Crate->Scale(0.06f, 0.06f, 0.06f);
	m_Crate->Multiply(m_Crate->GetScailingMatrix(), m_Crate->GetTranslationMatrix());

	m_Crate2 = new ModelClass;
	if (!m_Crate2) { return false; }

	result = m_Crate2->Initialize("../Dreamy/Data/Crate.txt", L"../Dreamy/Data/CrateTexture.dds");
	if (!result) { MessageBox(hwnd, L"m_Crate2", L"Error", MB_OK); return false; }

	m_Crate2->Translation(235.0f, 17.0f, 435.0f);
	m_Crate2->Scale(0.05f, 0.05f, 0.05f);
	m_Crate2->Multiply(m_Crate2->GetScailingMatrix(), m_Crate2->GetTranslationMatrix());

	//--------------------------------------------------------------------------------------

	//빌보딩표지판
	m_Billboarding = new ModelClass;

	result = m_Billboarding->Initialize("../Dreamy/Data/square2.txt", L"../Dreamy/Data/Billboard.png");
	if (!result) { MessageBox(hwnd, L"Could not initialize tree object", L"Error", MB_OK); return false; }

	m_Billboarding->Translation(480.0f, 105.0f, 537.0f);
	m_Billboarding->Scale(1.5f, 1.5f, 1.5f);

	//느낌표
	m_Mark = new ModelClass;

	result = m_Mark->Initialize("../Dreamy/Data/square2.txt", L"../Dreamy/Data/Mark.png");
	if (!result) { MessageBox(hwnd, L"Could not initialize tree object", L"Error", MB_OK); return false; }

	m_Mark->Translation(513.0f, 55.0f, 405.0f);
	m_Mark->Scale(0.4f, 0.4f, 0.4f);

	return true;
}
bool GraphicsClass::LandLoading(HWND hwnd)
{
	bool result;
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

	//m_TerrainShader2 = new TerrainShaderClass2;
	//if (!m_TerrainShader2) { return false; }
	//
	//m_TerrainShader2->Initialize(hwnd);
	//if (!result) { MessageBox(hwnd, L"Could not initialize TerrainShader object", L"Error", MB_OK); return false; }

	m_Sky = new SkyClass;
	if (!m_Sky) { return false; }

	result = m_Sky->InitializeSky();
	if (!result) { MessageBox(hwnd, L"Could not initialize Sky object", L"Error", MB_OK); return false; }

	result = m_Sky->InitializeCloud(L"../Dreamy/Data/cloud002.dds");
	if (!result) { MessageBox(hwnd, L"Could not initialize Cloud object", L"Error", MB_OK); return false; }

	//--------------------------------------------------------------------------------------

	// 그림자
	//-------------------------------------------------------------------------------------
	m_ShadowMap = new RTTTextureClass;
	if (!m_ShadowMap) { return false; }

	result = m_ShadowMap->Initialize(width, height, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result) { MessageBox(hwnd, L"Could not initialize the m_ShadowMap render to texture object.", L"Error", MB_OK); return false; }
	//-------------------------------------------------------------------------------------


	// 호수
	//-------------------------------------------------------------------------------------
	// Create the refraction render to texture object.
	m_RefractionTexture = new RTTTextureClass;
	if (!m_RefractionTexture) { return false; }

	// Initialize the refraction render to texture object.
	result = m_RefractionTexture->Initialize(width, height, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result) { MessageBox(hwnd, L"Could not initialize the refraction render to texture object.", L"Error", MB_OK); return false; }

	// Create the water object.
	m_Water = new WaterClass;
	if (!m_Water) { return false; }

	// Initialize the water object.
	result = m_Water->Initialize(L"../Dreamy/Data/waternormal.dds", 11.0f, 55.0f);
	if (!result) { MessageBox(hwnd, L"Could not initialize the water object.", L"Error", MB_OK); return false; }


	D3DXMatrixTranslation(&WaterWorldMatrix, 766.0f, m_Water->GetWaterHeight(), 470.0f);
	D3DXMatrixRotationY(&WaterRotationMatrix, 70.0f);
	D3DXMatrixMultiply(&WaterWorldMatrix, &WaterRotationMatrix, &WaterWorldMatrix);
	//-------------------------------------------------------------------------------------

	return true;
}
bool GraphicsClass::InstancingLoading(HWND hwnd)
{
	bool result;
	// 인스턴싱 객체들 생성
	//--------------------------------------------------------------------------------------
	m_Instancing = new InstancingClass;
	if (!m_Instancing) { return false; }

	m_Instancing->SetInstanceCount(7);
	m_Instancing->SetInsatanceVariable(-5.0f, 0.0f, 25.0f);
	m_Instancing->SetInstancePosition(140.0f, 9.5f, 470.0f);

	result = m_Instancing->Initialize("../Dreamy/Data/MapleTreeStem.txt", L"../Dreamy/Data/tree_bark.dds");
	if (!result) { MessageBox(hwnd, L"instancing", L"Error", MB_OK); return false; }

	m_Instancing2 = new InstancingClass;
	if (!m_Instancing2) { return false; }

	m_Instancing2->SetInstanceCount(7);
	m_Instancing2->SetInsatanceVariable(-5.0f, 0.0f, 25.0f);
	m_Instancing2->SetInstancePosition(140.0f, 9.5f, 470.0f);

	result = m_Instancing2->Initialize("../Dreamy/Data/MapleTreeLeaves.txt", L"../Dreamy/Data/green_leaf.dds");
	if (!result) { MessageBox(hwnd, L"instancing", L"Error", MB_OK); return false; }

	//----------------------
	m_Instancing3 = new InstancingClass;
	if (!m_Instancing3) { return false; }

	m_Instancing3->SetInstanceCount(8);
	m_Instancing3->SetInsatanceVariable(-25.0f, 0.0f, 0.0f);
	m_Instancing3->SetInstancePosition(315.0f, 16.0f, 350.0f);

	result = m_Instancing3->Initialize("../Dreamy/Data/MapleTreeStem.txt", L"../Dreamy/Data/tree_bark.dds");
	if (!result) { MessageBox(hwnd, L"instancing", L"Error", MB_OK); return false; }

	m_Instancing4 = new InstancingClass;
	if (!m_Instancing4) { return false; }

	m_Instancing4->SetInstanceCount(8);
	m_Instancing4->SetInsatanceVariable(-25.0f, 0.0f, 0.0f);
	m_Instancing4->SetInstancePosition(315.0f, 16.0f, 350.0f);

	result = m_Instancing4->Initialize("../Dreamy/Data/MapleTreeLeaves.txt", L"../Dreamy/Data/green_leaf.dds");
	if (!result) { MessageBox(hwnd, L"instancing", L"Error", MB_OK); return false; }

	//----------------------5~6 오른쪽에 있는애들
	m_Instancing5 = new InstancingClass;
	if (!m_Instancing5) { return false; }

	m_Instancing5->SetInstanceCount(9);
	m_Instancing5->SetInsatanceVariable(-25.0f, 0.0f, 0.0f);
	m_Instancing5->SetInstancePosition(310.0f, 10.0f, 650.0f);

	result = m_Instancing5->Initialize("../Dreamy/Data/MapleTreeStem.txt", L"../Dreamy/Data/tree_bark.dds");
	if (!result) { MessageBox(hwnd, L"instancing", L"Error", MB_OK); return false; }

	m_Instancing6 = new InstancingClass;
	if (!m_Instancing6) { return false; }

	m_Instancing6->SetInstanceCount(9);
	m_Instancing6->SetInsatanceVariable(-25.0f, 0.0f, 0.0f);
	m_Instancing6->SetInstancePosition(310.0f, 10.0f, 650.0f);

	result = m_Instancing6->Initialize("../Dreamy/Data/MapleTreeLeaves.txt", L"../Dreamy/Data/green_leaf.dds");
	if (!result) { MessageBox(hwnd, L"instancing", L"Error", MB_OK); return false; }

	//----------------------744 51 554 x축커지게
	m_Instancing7 = new InstancingClass;
	if (!m_Instancing7) { return false; }

	m_Instancing7->SetInstanceCount(5);
	m_Instancing7->SetInsatanceVariable(25.0f, 0.0f, 0.0f);
	m_Instancing7->SetInstancePosition(735.0f, 13.0f, 530.0f);

	result = m_Instancing7->Initialize("../Dreamy/Data/MapleTreeStem.txt", L"../Dreamy/Data/tree_bark.dds");
	if (!result) { MessageBox(hwnd, L"instancing", L"Error", MB_OK); return false; }

	m_Instancing8 = new InstancingClass;
	if (!m_Instancing8) { return false; }

	m_Instancing8->SetInstanceCount(5);
	m_Instancing8->SetInsatanceVariable(25.0f, 0.0f, 0.0f);
	m_Instancing8->SetInstancePosition(735.0f, 13.0f, 530.0f);

	result = m_Instancing8->Initialize("../Dreamy/Data/MapleTreeLeaves.txt", L"../Dreamy/Data/maple_leaf.dds");
	if (!result) { MessageBox(hwnd, L"instancing", L"Error", MB_OK); return false; }

	//----------------------
	m_Instancing9 = new InstancingClass;
	if (!m_Instancing9) { return false; }

	m_Instancing9->SetInstanceCount(7);
	m_Instancing9->SetInsatanceVariable(-5.0f, 0.0f, 25.0f);
	m_Instancing9->SetInstancePosition(350.0f, 9.5f, 470.0f);

	result = m_Instancing9->Initialize("../Dreamy/Data/MapleTreeStem.txt", L"../Dreamy/Data/tree_bark.dds");
	if (!result) { MessageBox(hwnd, L"instancing", L"Error", MB_OK); return false; }

	m_Instancing10 = new InstancingClass;
	if (!m_Instancing10) { return false; }

	m_Instancing10->SetInstanceCount(7);
	m_Instancing10->SetInsatanceVariable(-5.0f, 0.0f, 25.0f);
	m_Instancing10->SetInstancePosition(350.0f, 9.5f, 470.0f);

	result = m_Instancing10->Initialize("../Dreamy/Data/MapleTreeLeaves.txt", L"../Dreamy/Data/green_leaf.dds");
	if (!result) { MessageBox(hwnd, L"instancing", L"Error", MB_OK); return false; }

	//----------------------
	m_Instancing11 = new InstancingClass;
	if (!m_Instancing11) { return false; }

	m_Instancing11->SetInstanceCount(5);
	m_Instancing11->SetInsatanceVariable(0.0f, 0.0f, -25.0f);
	m_Instancing11->SetInstancePosition(840.0f, 13.0f, 517.0f);

	result = m_Instancing11->Initialize("../Dreamy/Data/MapleTreeStem.txt", L"../Dreamy/Data/tree_bark.dds");
	if (!result) { MessageBox(hwnd, L"instancing", L"Error", MB_OK); return false; }

	m_Instancing12 = new InstancingClass;
	if (!m_Instancing12) { return false; }

	m_Instancing12->SetInstanceCount(5);
	m_Instancing12->SetInsatanceVariable(0.0f, 0.0f, -25.0f);
	m_Instancing12->SetInstancePosition(840.0f, 13.0f, 517.0f);

	result = m_Instancing12->Initialize("../Dreamy/Data/MapleTreeLeaves.txt", L"../Dreamy/Data/maple_leaf.dds");
	if (!result) { MessageBox(hwnd, L"instancing", L"Error", MB_OK); return false; }

	//----------------------

	m_Instancing13 = new InstancingClass;
	if (!m_Instancing7) { return false; }

	m_Instancing13->SetInstanceCount(5);
	m_Instancing13->SetInsatanceVariable(25.0f, 0.0f, 0.0f);
	m_Instancing13->SetInstancePosition(735.0f, 13.0f, 392.0f);

	result = m_Instancing13->Initialize("../Dreamy/Data/MapleTreeStem.txt", L"../Dreamy/Data/tree_bark.dds");
	if (!result) { MessageBox(hwnd, L"instancing", L"Error", MB_OK); return false; }

	m_Instancing14 = new InstancingClass;
	if (!m_Instancing14) { return false; }

	m_Instancing14->SetInstanceCount(5);
	m_Instancing14->SetInsatanceVariable(25.0f, 0.0f, 0.0f);
	m_Instancing14->SetInstancePosition(735.0f, 13.0f, 392.0f);

	result = m_Instancing14->Initialize("../Dreamy/Data/MapleTreeLeaves.txt", L"../Dreamy/Data/maple_leaf.dds");
	if (!result) { MessageBox(hwnd, L"instancing", L"Error", MB_OK); return false; }
	//--------------------------------------------------------------------------------------
	return true;
}
bool GraphicsClass::ImageLoading(HWND hwnd)
{
	bool result;

	m_Loading = new ImageClass;

	result = m_Loading->Initialize(width, height, L"../Dreamy/Data/Loading.jpg", width, height);
	if (!result) { MessageBox(hwnd, L"Could not initialize model object", L"Error", MB_OK); return false; }

	RenderLoadingScene();

	m_Ending = new ImageClass;

	result = m_Ending->Initialize(width, height, L"../Dreamy/Data/End.jpg", width, height);
	if (!result) { MessageBox(hwnd, L"Could not initialize model object", L"Error", MB_OK); return false; }

	// 2D이미지
	//--------------------------------------------------------------------------------------
	m_CrossHair = new ImageClass;
	if (!m_CrossHair) { return false; }

	result = m_CrossHair->Initialize(width, height, L"../Dreamy/Data/Crosshair3.png", width, height);
	if (!result) { MessageBox(hwnd, L"Could not initialize Crosshair", L"Error", MB_OK); return false; }

	m_UIManager = new UIManagerClass;
	if (!m_UIManager) { return false; }

	m_UIManager->Initialize(width, height);
	if (!result) { MessageBox(hwnd, L"Could not initialize m_UIManager", L"Error", MB_OK); return false; }

	m_Balloon = new ImageClass;
	if (!m_Balloon) { return false; }

	result = m_Balloon->Initialize(width, height, L"../Dreamy/Data/Balloon.png", 88, 85);
	if (!result) { MessageBox(hwnd, L"ballon error", L"Error", MB_OK); return false; }
	//--------------------------------------------------------------------------------------

	// 미니맵
	//--------------------------------------------------------------------------------------
	m_Minimap = new MinimapClass;
	if (!m_Minimap) { return false; }

	result = m_Minimap->Initialize(width, height, 1025, 1025);
	if (!result) { MessageBox(hwnd, L"minimap error", L"Error", MB_OK); return false; }
	//--------------------------------------------------------------------------------------

	// Text
	//--------------------------------------------------------------------------------------
	m_Text = new TextClass;
	if (!m_Text) { return false; }

	result = m_Text->Initialize(hwnd, width, height, ImageViewMatrix);
	if (!result) { MessageBox(hwnd, L"m_Text error", L"Error", MB_OK); return false; }
	//--------------------------------------------------------------------------------------


	return true;
}
bool GraphicsClass::EffectLoading(HWND hwnd)
{
	bool result;
	// 빌보딩 객체(파티클 등)
	//--------------------------------------------------------------------------------------
	m_Particle = new ParticleSystem;
	if (!m_Particle) { return false; }

	result = m_Particle->Initialize(L"../Dreamy/Data/star.dds");
	if (!result) { MessageBox(hwnd, L"Could not particle TitleText", L"Error", MB_OK); return false; }

	//집에 난 불
	m_Fire = new ModelClass;

	result = m_Fire->InitializeTriple("../Dreamy/Data/square.txt", L"../Dreamy/Data/fire.dds", L"../Dreamy/Data/noise01.dds", L"../Dreamy/Data/alpha01.dds");
	if (!result) { MessageBox(hwnd, L"Could not initialize fire effect object", L"Error", MB_OK); return false; }

	m_Fire->Translation(157.0f, 76.0f, 429.0f);
	//m_Fire->RotationY(CalculateBillboarding(CameraPos, firePosition));
	m_Fire->RotationY(29.7f);
	m_Fire->Scale(40.0f, 60.0f, 40.0f);

	m_Fire->Multiply(m_Fire->GetScailingMatrix(), m_Fire->GetRotationYMatrix());
	m_Fire->Multiply(m_Fire->GetFinalMatrix(), m_Fire->GetTranslationMatrix());
	m_Fire->active = true;

	//나무1에 난 불
	m_TFire = new ModelClass;

	result = m_TFire->InitializeTriple("../Dreamy/Data/square.txt", L"../Dreamy/Data/fire.dds", L"../Dreamy/Data/noise01.dds", L"../Dreamy/Data/alpha01.dds");
	if (!result) { MessageBox(hwnd, L"Could not initialize m_TFire effect object", L"Error", MB_OK); return false; }

	m_TFire->Translation(130.0f, 56.0f, 521.0f);
	m_TFire->Scale(20.0f, 30.0f, 20.0f);
	m_TFire->active = true;

	//나무3에 난 불
	m_TFire2 = new ModelClass;

	result = m_TFire2->InitializeTriple("../Dreamy/Data/square.txt", L"../Dreamy/Data/fire.dds", L"../Dreamy/Data/noise01.dds", L"../Dreamy/Data/alpha01.dds");
	if (!result) { MessageBox(hwnd, L"Could not initialize m_TFire2 effect object", L"Error", MB_OK); return false; }

	m_TFire2->Translation(265.0f, 60.0f, 348.0f);
	m_TFire2->Scale(20.0f, 30.0f, 20.0f);

	m_TFire2->active = true;

	//나무5에 난 불
	m_TFire3 = new ModelClass;

	result = m_TFire3->InitializeTriple("../Dreamy/Data/square.txt", L"../Dreamy/Data/fire.dds", L"../Dreamy/Data/noise01.dds", L"../Dreamy/Data/alpha01.dds");
	if (!result) { MessageBox(hwnd, L"Could not initialize m_TFire3 effect object", L"Error", MB_OK); return false; }

	m_TFire3->Translation(160.0f, 56.0f, 651.0f);
	m_TFire3->Scale(20.0f, 30.0f, 30.0f);

	m_TFire3->active = true;

	//나무9에 난 불
	m_TFire4 = new ModelClass;

	result = m_TFire4->InitializeTriple("../Dreamy/Data/square.txt", L"../Dreamy/Data/fire.dds", L"../Dreamy/Data/noise01.dds", L"../Dreamy/Data/alpha01.dds");
	if (!result) { MessageBox(hwnd, L"Could not initialize m_TFire4 effect object", L"Error", MB_OK); return false; }

	m_TFire4->Translation(345.0f, 55.0f, 493.0f);
	m_TFire4->Scale(20.0f, 30.0f, 30.0f);

	m_TFire4->active = true;

	//242.0f, 18.5f, 568.5f);
	//Barrel에 난 불 
	m_BFire = new ModelClass;

	result = m_BFire->InitializeTriple("../Dreamy/Data/square.txt", L"../Dreamy/Data/fire.dds", L"../Dreamy/Data/noise01.dds", L"../Dreamy/Data/alpha01.dds");
	if (!result) { MessageBox(hwnd, L"Could not initialize m_BFire effect object", L"Error", MB_OK); return false; }

	m_BFire->Translation(250.0f, 53.0f, 568.5f);
	m_BFire->Scale(30.0f, 40.0f, 30.0f);

	m_BFire->active = true;

	//--------------------------------------------------------------------------------------

	return true;
}
bool GraphicsClass::OtherLoading(HWND hwnd)
{
	//타이머
	m_Timer = new FpsClass;
	if (!m_Timer) { return false; }
	m_Timer->Initialize();

	// Light
	m_Light = new LightClass;
	if (!m_Light) { return false; }

	m_Light->SetAmbientColor(0.05f, 0.05f, 0.05f, 1.0f);
	//m_Light->SetAmbientColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetPosition(0.2f, -1.0f, 1.0f);
	m_Light->SetLookAt(0.0f, 0.0f, 0.0f);
	m_Light->GenerateProjectionMatrix(SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT);
	// Get the view and orthographic matrices from the light object.
	m_Light->GenerateViewMatrix();
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);

	m_Light->SetPosition(0.5f, -1.0f, -0.25f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(16.0f); // 반사강도값이 낮을수록 반사광 효과가 커진다!


									  // Frustum
	m_Frustum = new FrustumClass;
	if (!m_Frustum) { return false; }

	return true;
}

bool GraphicsClass::Loading(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	// 로딩이미지 객체 생성 및 출력
	//-------------------------------------------------------------------------------------
	SAFE_SHUTDOWN(m_Start);

	result = ImageLoading(hwnd);
	if (!result) { MessageBox(hwnd, L"Could not initialize 2DImage object", L"Error", MB_OK); return false; }

	result = FBXLoading();
	if (!result) { MessageBox(hwnd, L"Could not initialize fbx object", L"Error", MB_OK); return false; }

	result = OBJLoading(hwnd);
	if (!result) { MessageBox(hwnd, L"Could not initialize obj object", L"Error", MB_OK); return false; }

	result = LandLoading(hwnd);
	if (!result) { MessageBox(hwnd, L"Could not initialize landscape object", L"Error", MB_OK); return false; }

	result = InstancingLoading(hwnd);
	if (!result) { MessageBox(hwnd, L"Could not initialize Instancing object", L"Error", MB_OK); return false; }

	result = EffectLoading(hwnd);
	if (!result) { MessageBox(hwnd, L"Could not initialize effect object", L"Error", MB_OK); return false; }

	result = OtherLoading(hwnd);
	if (!result) { MessageBox(hwnd, L"Could not initialize Other object", L"Error", MB_OK); return false; }

	return true;
}

// 엔딩 후 엔딩장면에 필요없는것들을 해제한다.
void GraphicsClass::PreShutdown()
{
	FBXShader::DeleteBuffer();
	SAFE_SHUTDOWN(m_UIManager);
	SAFE_SHUTDOWN(m_Minimap);
	SAFE_DELETE(m_horse);
	SAFE_DELETE(m_horse2);
	SAFE_DELETE(m_horse3);
	SAFE_DELETE(m_npc);
	SAFE_SHUTDOWN(m_Billboarding);
	SAFE_SHUTDOWN(m_Mark);
	SAFE_DELETE(m_Light);
	SAFE_SHUTDOWN(m_Crate);
	SAFE_SHUTDOWN(m_Crate2);
	SAFE_SHUTDOWN(m_Barrel);
	SAFE_SHUTDOWN(m_Barrel2);
	SAFE_SHUTDOWN(m_Barrel3);
	SAFE_SHUTDOWN(m_Fire);
	SAFE_SHUTDOWN(m_TFire);
	SAFE_SHUTDOWN(m_TFire2);
	SAFE_SHUTDOWN(m_TFire3);
	SAFE_SHUTDOWN(m_TFire4);
	SAFE_SHUTDOWN(m_BFire);
	SAFE_SHUTDOWN(m_circle);
	SAFE_SHUTDOWN(m_TerrainShader);
	//SAFE_SHUTDOWN(m_TerrainShader2); 243 54 510
	SAFE_SHUTDOWN(m_Water);
	SAFE_SHUTDOWN(m_Balloon);
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
	SAFE_SHUTDOWN(m_Instancing11);
	SAFE_SHUTDOWN(m_Instancing12);
	SAFE_SHUTDOWN(m_Instancing13);
	SAFE_SHUTDOWN(m_Instancing14);
	SAFE_SHUTDOWN(m_Terrain);
	SAFE_SHUTDOWN(m_House);
	SAFE_SHUTDOWN(m_Sky);
	SAFE_SHUTDOWN(m_NpcBox);
	SAFE_SHUTDOWN(m_WaterBox);
	SAFE_SHUTDOWN(m_CrossHair);
	SAFE_DELETE(m_Frustum);
	SAFE_SHUTDOWN(m_MouseCursor);
	m_Text->Shutdown();
	SAFE_SHUTDOWN(m_Particle);
	SAFE_SHUTDOWN(m_Loading);
	SAFE_DELETE(m_Timer);
}

void GraphicsClass::Shutdown()
{
	m_Text->EndShutdown();
	delete(m_Text);
	SAFE_SHUTDOWN(m_Ending);
	Rasterizer::Delete();
	Blender::Delete();
	DepthStencil::Delete();
	Camera::Delete();
	D3D::Delete();
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
bool GraphicsClass::Frame(int fps, float frameTime, D3DXVECTOR3 pos, D3DXVECTOR3 rot, int mouseX, int mouseY, bool space)
{
	bool result;
	bool foundHeight;

	if (click == false)
		n_click = 0;

	if (m_circle->active == true)
		m_Timer->Frame();

	// Camera Frame처리
	//--------------------------------------------------------------------------------------

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
	result = m_Text->SetTime(m_Timer->GetTime());
	if (!result) { return false; }
	m_endingTime = m_Timer->GetTime();
	//--------------------------------------------------------------------------------------

	// 물풍선
	//--------------------------------------------------------------------------------------
	if (space == true)
		shoot = true;

	if (shoot == true && m_circle->active == true && (Balloon1 == true || Balloon2 == true || Balloon3 == true))
		Shoot(frameTime);
	//--------------------------------------------------------------------------------------

	// 물풍선 처리 
	//--------------------------------------------------------------------------------------
	BalloonFrame();
	//--------------------------------------------------------------------------------------

	 // Model들 Frame처리
	 // 프레임 처리 메모리에 직접타격있으니까 제발 액티브에서만하자!!!!!
	 //--------------------------------------------------------------------------------------
	m_Sky->Frame(frameTime*0.00001f, 0.0f, frameTime*0.00002f, 0.0f); //구름
	m_Minimap->PositionUpdate(CameraPos.x, CameraPos.z); //미니맵

	//불
	if (m_Fire->active == true || m_TFire->active == true || m_TFire2->active == true || m_TFire3->active ==true || m_TFire4->active==true || m_BFire->active==true)
		SetEffectVariable(frameTime*0.0005f);

	//호수
	if (m_Water->active == true)
		m_Water->Frame(frameTime);

	// 파티클
	if (m_Particle->active == true)
		m_Particle->Frame(frameTime);
	else
		Fparticle = TFparticle = TF2particle = TF3particle = TF4particle = BFparticle =false;

	// 말들
	HorseFrame(frameTime);


	//--------------------------------------------------------------------------------------

	return true;
}
/*------------------------------------------------------------------------------------------
이름: ~~Frame()
용도: 모델들의 frame처리 관련 함수들을 따로 뺌.
------------------------------------------------------------------------------------------*/
void GraphicsClass::HorseFrame(float frameTime)
{
	//말1
	if (starthorse == true)
	{
		float horseY;
		if (m_Terrain->GetHeightAtPosition(horsePos.x, horsePos.z, horseY))
			horsePos.y = horseY + 2.0f;

		m_horse->Update(1.0f);
		//800

		if (m_horse->turn == false)
			horsePos.x = horsePos.x + frameTime*0.05f;
		else
			horsePos.x = horsePos.x - frameTime*0.05f;

		if (horsePos.x >= 700.0f)
			m_horse->turn = true;

		if (horsePos.x <= 330.0f)
			m_horse->turn = false;
	}

	//609 120
	//말2
	if (starthorse2 == true)
	{
		float horse2Y;
		if (m_Terrain->GetHeightAtPosition(horse2Pos.x, horse2Pos.z, horse2Y))
			horse2Pos.y = horse2Y + 2.0f;

		//800
		m_horse2->Update(1.0f);

		if (m_horse2->turn == false)
		{
			horse2Pos.x = horse2Pos.x + frameTime*0.04f;
			horse2Pos.z = horse2Pos.z - frameTime*0.04f;
		}
		else
		{
			horse2Pos.x = horse2Pos.x - frameTime*0.04f;
			horse2Pos.z = horse2Pos.z + frameTime*0.04f;
		}

		if (horse2Pos.x >= 605 && horse2Pos.z <= 125)
			m_horse2->turn = true;
		if (horse2Pos.x <= 233 && horse2Pos.z >= 502)
			m_horse2->turn = false;
	}

	//말3
	if (starthorse3 == true)
	{
		float horse3Y;
		if (m_Terrain->GetHeightAtPosition(horse3Pos.x, horse3Pos.z, horse3Y))
			horse3Pos.y = horse3Y + 2.0f;

		//800
		m_horse3->Update(1.0f);

		if (m_horse3->turn == false)
		{
			horse3Pos.x = horse3Pos.x - frameTime*0.02f;
			horse3Pos.z = horse3Pos.z - frameTime*0.04f;
		}
		else
		{
			horse3Pos.x = horse3Pos.x + frameTime*0.02f;
			horse3Pos.z = horse3Pos.z + frameTime*0.04f;
		}

		if (horse3Pos.x <= 540.0f && horse3Pos.z <= 185.0f)
			m_horse3->turn = true;
		if (horse3Pos.x >= 671.0f  && horse3Pos.z >= 464.0f)
			m_horse3->turn = false;
	}

	//npc
	if (renderNpc == true)
		m_npc->Update(0.5f);
}
void GraphicsClass::BalloonFrame()
{
	// 157.0f, 49.0f, 429.0f
	if (shoot == true && m_circle->active == true && m_Fire->active == true && CameraPos.x <= 215.0f && CameraPos.z >= 385.0f && CameraPos.z <= 465.0f)
	{
		if (dx == 94)
		{
			isEffectSound = true;
			m_Particle->active = true;
			Fparticle = true;
			m_Fire->active = false;
			OffFire = true;

		}

	}
	if (shoot == true && m_circle->active == true && m_TFire->active == true && CameraPos.x <= 180.0f &&CameraPos.z >= 490.0f&& CameraPos.z <= 560.0f)
	{
		if (dx == 94)
		{
			isEffectSound = true;
			m_Particle->active = true;
			TFparticle = true;
			m_TFire->active = false;
			OffTFire = true;
		}
	}

	//230 280
	if (shoot == true && m_circle->active == true && m_TFire2->active == true && CameraPos.z <= 385.0f && CameraPos.x >= 230.0f && CameraPos.x <= 280.0f)
	{
		if (dx == 94)
		{
			isEffectSound = true;
			m_Particle->active = true;
			TF2particle = true;
			m_TFire2->active = false;
			OffTFire2 = true;
		}
	}

	if (shoot == true && m_circle->active == true && m_TFire2->active == true && CameraPos.z <= 385.0f && CameraPos.x >= 230.0f && CameraPos.x <= 280.0f)
	{
		if (dx == 94)
		{
			isEffectSound = true;
			m_Particle->active = true;
			TF2particle = true;
			m_TFire2->active = false;
			OffTFire2 = true;
		}
	}

	if (shoot == true && m_circle->active == true && m_TFire3->active == true && CameraPos.z >= 615.0f && CameraPos.x >= 130.0f && CameraPos.x <= 190.0f)
	{
		if (dx == 94)
		{
			isEffectSound = true;
			m_Particle->active = true;
			TF3particle = true;
			m_TFire3->active = false;
			OffTFire3 = true;
		}
	}

	if (shoot == true && m_circle->active == true && m_TFire4->active == true && CameraPos.x <= 370.0f && CameraPos.x >= 310.0f && CameraPos.z >= 470.0f && CameraPos.z <= 495.0f)
	{
		if (dx == 94)
		{
			isEffectSound = true;
			m_Particle->active = true;
			TF4particle = true;
			m_TFire4->active = false;
			OffTFire4 = true;
		}
	}

	if (shoot == true && m_circle->active == true && m_BFire->active == true && CameraPos.x <= 280.0f && CameraPos.x >= 210.0f && CameraPos.z >= 535.0f && CameraPos.z <= 605.0f)
	{
		if (dx == 94)
		{
			isEffectSound = true;
			m_Particle->active = true;
			BFparticle = true;
			m_BFire->active = false;
			OffBFire = true;
		}
	}
}
void GraphicsClass::Shoot(float frametime)
{

		vAccell = vGravity;

		if (dx <= 20)
			vVelocity = vVelocity;
		else
			vVelocity = vVelocity + vAccell*frametime*0.00085f;

		vPosition = vPosition + vVelocity *frametime*0.00010f;

		dy = vPosition.y;
		dz = vPosition.z;

		D3DXMatrixTranslation(&ShootMatrix, 0.0f, dy, dz);
		D3DXMatrixMultiply(&BalloonWorldMatrix, &BalloonWorldMatrix, &ShootMatrix);

		dx++;

		if (dx == 95)
		{
			shoot = false;
			dx = 0;
			dy = 0.0f;
			dz = 0.0f;
			vAccell = { 0.0f, 0.0f, 0.0f };
			vPosition = { 0.0f, 0.0f, 0.0f };
			vVelocity = { 0.0f,3.0f,1.5f };
			D3DXMatrixTranslation(&ShootMatrix, 0.0f, dy, dz);
			D3DXMatrixIdentity(&BalloonWorldMatrix);

			if (Shoot3)
				Balloon3 = false;
			else if (Shoot2)
				Balloon2 = false;
			else if (Shoot1)
				Balloon1 = false;	
		}
	
}


/*----------------------------------------------------------------------------------------
이름 : Render()
용도 : 러닝씬과 RTT씬을 그린다.
- 반사를 할 때는 우선 RTT를 먼저하고 러닝씬을 로드해야 혼합된다.
------------------------------------------------------------------------------------------*/
bool GraphicsClass::Render()
{
	//기본행렬
	//-------------------------------------------------------------------------------------
	D3DXMatrixIdentity(&worldMatrix);
	Camera::Get()->GetView(&viewMatrix);
	Camera::Get()->GetProjection(&projectionMatrix);
	Camera::Get()->GetOrtho(&orthoMatrix);

	bool result;

	//그림자
	//-------------------------------------------------------------------------------------
	//result = RenderShadowToTexture(Pressed);
	//if (!result) { return false; }

	// 호수
	//-------------------------------------------------------------------------------------
	if (m_Water->active == true)
	{
		result = RenderRefractionToTexture();
		if (!result) { return false; }
	}

	// 메인 Scene
	//-------------------------------------------------------------------------------------
	result = RenderRunningScene();
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
bool GraphicsClass::RenderRunningScene()
{
	bool result;
	bool renderModel;

	//안개 객체 초기화
	//fogEnd가 멀어질수록 밝아짐
	//-------------------------------------------------------------------------------------
	float fogColor, fogStart, fogEnd;
	fogColor = 0.0f;
	fogStart = 600.0f;
	fogEnd = 650.0f;
	//-------------------------------------------------------------------------------------



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
	D3DXMatrixTranslation(&SkyworldMatrix, CameraPos.x, CameraPos.y, CameraPos.z);

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
		m_Sky->GetCloudTexture1(), m_Sky->GetTranslation(0), m_Sky->GetTranslation(1),
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
			//result = m_TerrainShader2->Render(m_Terrain->GetCellIndexCount(i), worldMatrix, viewMatrix,
			//	projectionMatrix,ImageViewMatrix, ImageProjectionMatrix, m_Terrain->GetColorTexture(), m_Terrain->GetNormalMapTexture(), m_ShadowMap->GetShaderResourceView(),
			//	D3DXVECTOR3(0.2, -1, 1), m_Light->GetDiffuseColor(), fogStart, fogEnd);
			if (!result) { return false; }

		}
	}
	//-------------------------------------------------------------------------------------

	// 호수  512.5f, 30.0f, 310.0f
	//-------------------------------------------------------------------------------------
	m_Water->active = m_Frustum->CheckSphere(766.0f, m_Water->GetWaterHeight(), 470.0f, 10.0f);
	if (m_Water->active)
	{
		WaterreflectionViewMatrix = Camera::Get()->RenderWaterReflection(m_Water->GetWaterHeight());
		m_Water->Render();

		result = m_Shader->RenderWaterShader(m_Water->GetIndexCount(), WaterWorldMatrix, viewMatrix, projectionMatrix, WaterreflectionViewMatrix,
			m_RefractionTexture->GetShaderResourceView(), m_Water->GetTexture(),
			CameraPos, m_Water->GetNormalMapTiling(), m_Water->GetWaterTranslation(), m_Water->GetReflectRefractScale(),
			m_Water->GetRefractionTint(), m_Light->GetDirection(), m_Water->GetSpecularShininess());
		if (!result) { return false; }
	}
	//-------------------------------------------------------------------------------------

	// FBXANIMATIONModel들
	//-------------------------------------------------------------------------------------

	//npc
	renderNpc = m_Frustum->CheckCube(515.0f, 25.0f, 415.0f, 10.0f);
	if (renderNpc == true)
	{
		m_npc->Render();
	}

	//말1
	if (renderhorse = m_Frustum->CheckSphere(horsePos.x, horsePos.y, horsePos.z, 3.0f))
		starthorse = true;

	
	if (starthorse == true)
	{
		D3DXMATRIX ROT;

		if(m_horse->turn==false)
			D3DXMatrixRotationAxis(&ROT, &D3DXVECTOR3(0, 1, 0), 1.2f);
		else
			D3DXMatrixRotationAxis(&ROT, &D3DXVECTOR3(0, 1, 0), 5.0f);

		m_horse->Translation(horsePos.x, horsePos.y, horsePos.z);

		m_horse->Multiply(m_horse->GetScailingMatrix(), ROT);
		//m_horse->Multiply(m_horse->GetScailingMatrix(), m_horse->GetTranslationMatrix());
		m_horse->Multiply(m_horse->GetFinalMatrix(), m_horse->GetTranslationMatrix());
		
		m_horse->SetWorldTransform(m_horse->GetFinalMatrix());
		m_horse->Render();

	}

	//말2
	if(renderhorse2 = m_Frustum->CheckSphere(horse2Pos.x, horse2Pos.y, horse2Pos.z, 3.0f))
		 starthorse2 = true;

	if (starthorse2 == true)
	{
	D3DXMATRIX ROT2;
	if(m_horse2->turn==false)
		D3DXMatrixRotationAxis(&ROT2, &D3DXVECTOR3(0, 1, 0), 2.5f);
	else
		D3DXMatrixRotationAxis(&ROT2, &D3DXVECTOR3(0, 1, 0), 5.5f);

	m_horse2->Translation(horse2Pos.x, horse2Pos.y, horse2Pos.z);
	m_horse2->Multiply(m_horse2->GetScailingMatrix(), ROT2);
	m_horse2->Multiply(m_horse2->GetFinalMatrix(), m_horse2->GetTranslationMatrix());
		   
	m_horse2->SetWorldTransform(m_horse2->GetFinalMatrix());
		   
	m_horse2->Render();
	}

	//말3
	if (renderhorse3 = m_Frustum->CheckSphere(horse3Pos.x, horse3Pos.y, horse3Pos.z, 3.0f))
		starthorse3 = true;

	if (starthorse3 == true)
	{
		D3DXMATRIX ROT3;
		if (m_horse3->turn == false)
			D3DXMatrixRotationAxis(&ROT3, &D3DXVECTOR3(0, 1, 0), 2.5f);
		else
			D3DXMatrixRotationAxis(&ROT3, &D3DXVECTOR3(0, 1, 0), 5.5f);

		m_horse3->Translation(horse3Pos.x, horse3Pos.y, horse3Pos.z);
		m_horse3->Multiply(m_horse3->GetScailingMatrix(), ROT3);
		m_horse3->Multiply(m_horse3->GetFinalMatrix(), m_horse3->GetTranslationMatrix());

		m_horse3->SetWorldTransform(m_horse3->GetFinalMatrix());

		m_horse3->Render();
	}

	// OBJ모델
	//-------------------------------------------------------------------------------------

	// 물풍선 766 450
	//-----------------------------
	//m_circle->active = m_Frustum->CheckSphere(660.0f, 48.0f, 383.0f, 3.0f);

	m_circle->Render();
	if (m_circle->active == false)
	{
		//셰이더 렌더링
		result = m_Shader->RenderLightShader(m_circle->GetIndexCount(), m_circle->GetFinalMatrix(), viewMatrix, projectionMatrix
			, m_circle->GetTexture(),D3DXVECTOR3(0.2,-1,1), D3DXVECTOR4(0.9f, 0.9f, 0.9f, 1.0f), m_Light->GetDiffuseColor()
			, CameraPos, m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
		//result = m_Shader->RenderShadowShader(m_circle->GetIndexCount(), m_circle->GetFinalMatrix(), viewMatrix, projectionMatrix
		//	, lightViewMatrix, lightProjectionMatrix, m_circle->GetTexture(), m_ShadowMap->GetShaderResourceView(), D3DXVECTOR3(0.2, -1, 1)
		//	, D3DXVECTOR4(0.9f, 0.9f, 0.9f, 1.0f), m_Light->GetDiffuseColor());
		if (!result) { return false; }
	}
	else
		result = m_Shader->RenderLightShader(m_circle->GetIndexCount(), BalloonWorldMatrix, BalloonViewMatrix, projectionMatrix
			, m_circle->GetTexture(), D3DXVECTOR3(0.2, -1, 1), D3DXVECTOR4(0.9f, 0.9f, 0.9f, 1.0f), m_Light->GetDiffuseColor(), CameraPos
			, m_Light->GetSpecularColor(), m_Light->GetSpecularPower());


	

	// npc 충돌 박스
	//-----------------------------
	m_NpcBox->active = m_Frustum->CheckPoint(515.0f, 30.0f, 415.0f);
	
	if (m_NpcBox->active== true)
		m_NpcBox->Render();

	// 물 충돌 박스
	//-----------------------------
	if (m_Water->active)
		m_WaterBox->Render();
	
	
	// 집
	//-----------------------------
	m_House->active = m_Frustum->CheckCube(137.0f, 49.0f, 425.0f, 25.0f);

	if (m_House->active == true)
	{
		m_House->Render();

		if (OffFire == false)
			result = m_Shader->RenderDiffuseShader(m_House->GetIndexCount(), m_House->GetFinalMatrix(), viewMatrix, projectionMatrix, m_House->GetTexture(), D3DXVECTOR3(-0.5f, -1.0f, 0.0f), D3DXVECTOR4(0.75f, 0.75f, 0.75f, 1.0f));
		else
			result = m_Shader->RenderDiffuseShader(m_House->GetIndexCount(), m_House->GetFinalMatrix(), viewMatrix, projectionMatrix, m_House->GetTexture(), D3DXVECTOR3(-0.5f, -1.0f, 0.0f), D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));
	}
	// Barrel
	//-----------------------------
	m_Barrel->Render();
	m_Barrel2->Render();
	m_Barrel3->Render();

	if (OffBFire == false)
	{
		result = m_Shader->RenderDiffuseShader(m_Barrel->GetIndexCount(), m_Barrel->GetFinalMatrix(), viewMatrix, projectionMatrix, m_Barrel->GetTexture(), D3DXVECTOR3(-0.5f, -1.0f, 0.0f), D3DXVECTOR4(0.75f, 0.75f, 0.75f, 1.0f));
		result = m_Shader->RenderDiffuseShader(m_Barrel2->GetIndexCount(), m_Barrel2->GetFinalMatrix(), viewMatrix, projectionMatrix, m_Barrel2->GetTexture(), D3DXVECTOR3(-0.5f, -1.0f, 0.0f), D3DXVECTOR4(0.75f, 0.75f, 0.75f, 1.0f));
		result = m_Shader->RenderDiffuseShader(m_Barrel3->GetIndexCount(), m_Barrel3->GetFinalMatrix(), viewMatrix, projectionMatrix, m_Barrel3->GetTexture(), D3DXVECTOR3(-0.5f, -1.0f, 0.0f), D3DXVECTOR4(0.75f, 0.75f, 0.75f, 1.0f));
	}
	else
	{
		result = m_Shader->RenderDiffuseShader(m_Barrel->GetIndexCount(), m_Barrel->GetFinalMatrix(), viewMatrix, projectionMatrix, m_Barrel->GetTexture(), D3DXVECTOR3(-0.5f, -1.0f, 0.0f), D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));
		result = m_Shader->RenderDiffuseShader(m_Barrel2->GetIndexCount(), m_Barrel2->GetFinalMatrix(), viewMatrix, projectionMatrix, m_Barrel2->GetTexture(), D3DXVECTOR3(-0.5f, -1.0f, 0.0f), D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));
		result = m_Shader->RenderDiffuseShader(m_Barrel3->GetIndexCount(), m_Barrel3->GetFinalMatrix(), viewMatrix, projectionMatrix, m_Barrel3->GetTexture(), D3DXVECTOR3(-0.5f, -1.0f, 0.0f), D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));
	}
	//-------------------------------------------------------------------------------------

	// Crate
	//-------------------------------------------------------------------------------------
	m_Crate->Render();
	result = m_Shader->RenderTextureShader(m_Crate->GetIndexCount(), m_Crate->GetFinalMatrix(), viewMatrix, projectionMatrix, m_Crate->GetTexture());

	m_Crate2->Render();
	result = m_Shader->RenderTextureShader(m_Crate2->GetIndexCount(), m_Crate2->GetFinalMatrix(), viewMatrix, projectionMatrix, m_Crate2->GetTexture());
	//-------------------------------------------------------------------------------------

	// 인스턴싱, 빌보딩객체
	//-------------------------------------------------------------------------------------

	// 인스턴싱
	//-----------------------------
	Rasterizer::Get()->SetOffCullMode();

	if (OffTFire == false)
	{
		m_Instancing->Render(); 
		result = m_Shader->RenderDiffuseInstancingShader(m_Instancing->GetVertexCount(), m_Instancing->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing->GetTexture(), D3DXVECTOR3(-0.5f, -1.0f, 0.0f), D3DXVECTOR4(0.75f, 0.75f, 0.75f, 1.0f));

		m_Instancing2->Render();
		result = m_Shader->RenderDiffuseInstancingShader(m_Instancing2->GetVertexCount(), m_Instancing2->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing2->GetTexture(), D3DXVECTOR3(-0.5f, -1.0f, 0.0f), D3DXVECTOR4(0.75f, 0.75f, 0.75f, 1.0f));
	}
	else
	{
		Blender::Get()->SetLinear();

		m_Instancing->Render();
		result = m_Shader->RenderInstancingShader(m_Instancing->GetVertexCount(), m_Instancing->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing->GetTexture());

		m_Instancing2->Render();
		result = m_Shader->RenderInstancingShader(m_Instancing2->GetVertexCount(), m_Instancing2->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing2->GetTexture());

		Blender::Get()->SetOff();
	}

	if (OffTFire2 == false)
	{
		m_Instancing3->Render();
		result = m_Shader->RenderDiffuseInstancingShader(m_Instancing3->GetVertexCount(), m_Instancing3->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing3->GetTexture(), D3DXVECTOR3(-0.5f, -1.0f, 0.0f), D3DXVECTOR4(0.75f, 0.75f, 0.75f, 1.0f));

		m_Instancing4->Render();
		result = m_Shader->RenderDiffuseInstancingShader(m_Instancing4->GetVertexCount(), m_Instancing4->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing4->GetTexture(), D3DXVECTOR3(-0.5f, -1.0f, 0.0f), D3DXVECTOR4(0.75f, 0.75f, 0.75f, 1.0f));
	}

	else
	{
		Blender::Get()->SetLinear();
		m_Instancing3->Render();
		result = m_Shader->RenderInstancingShader(m_Instancing3->GetVertexCount(), m_Instancing3->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing3->GetTexture());

		m_Instancing4->Render();
		result = m_Shader->RenderInstancingShader(m_Instancing4->GetVertexCount(), m_Instancing4->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing4->GetTexture());

		Blender::Get()->SetOff();
	}

	if (OffTFire3 == false)
	{
		m_Instancing5->Render();
		result = m_Shader->RenderDiffuseInstancingShader(m_Instancing5->GetVertexCount(), m_Instancing5->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing5->GetTexture(), D3DXVECTOR3(-0.5f, -1.0f, 0.0f), D3DXVECTOR4(0.75f, 0.75f, 0.75f, 1.0f));
		if (!result) { return false; }

		m_Instancing6->Render();
		result = m_Shader->RenderDiffuseInstancingShader(m_Instancing6->GetVertexCount(), m_Instancing6->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing6->GetTexture(), D3DXVECTOR3(-0.5f, -1.0f, 0.0f), D3DXVECTOR4(0.75f, 0.75f, 0.75f, 1.0f));
		if (!result) { return false; }
	}
	else
	{
		Blender::Get()->SetLinear();

		m_Instancing5->Render();
		result = m_Shader->RenderInstancingShader(m_Instancing5->GetVertexCount(), m_Instancing5->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing5->GetTexture());

		m_Instancing6->Render();
		result = m_Shader->RenderInstancingShader(m_Instancing6->GetVertexCount(), m_Instancing6->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing6->GetTexture());

		Blender::Get()->SetOff();
	}

	if (OffTFire4 == false)
	{
		m_Instancing9->Render();
		result = m_Shader->RenderDiffuseInstancingShader(m_Instancing9->GetVertexCount(), m_Instancing9->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing9->GetTexture(), D3DXVECTOR3(-0.9f, -1.0f, 0.0f), D3DXVECTOR4(0.79f, 0.79f, 0.79f, 1.0f));
		if (!result) { return false; }

		m_Instancing10->Render();
		result = m_Shader->RenderDiffuseInstancingShader(m_Instancing10->GetVertexCount(), m_Instancing10->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing10->GetTexture(), D3DXVECTOR3(-0.10f, -1.0f, 0.0f), D3DXVECTOR4(0.710f, 0.710f, 0.710f, 1.0f));
		if (!result) { return false; }
	}
	else
	{
		Blender::Get()->SetLinear();
		m_Instancing9->Render();
		result = m_Shader->RenderInstancingShader(m_Instancing9->GetVertexCount(), m_Instancing9->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing9->GetTexture());
		if (!result) { return false; }

		m_Instancing10->Render();
		result = m_Shader->RenderInstancingShader(m_Instancing10->GetVertexCount(), m_Instancing10->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing10->GetTexture());
		if (!result) { return false; }
		Blender::Get()->SetOff();
	}

	Blender::Get()->SetLinear();

	m_Instancing7->Render();
	result = m_Shader->RenderInstancingShader(m_Instancing7->GetVertexCount(), m_Instancing7->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing7->GetTexture());
	if (!result) { return false; }

	m_Instancing8->Render();
	result = m_Shader->RenderInstancingShader(m_Instancing8->GetVertexCount(), m_Instancing8->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing8->GetTexture());
	if (!result) { return false; }



	m_Instancing11->Render();
	result = m_Shader->RenderInstancingShader(m_Instancing11->GetVertexCount(), m_Instancing11->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing11->GetTexture());
	if (!result) { return false; }

	m_Instancing12->Render();
	result = m_Shader->RenderInstancingShader(m_Instancing12->GetVertexCount(), m_Instancing12->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing12->GetTexture());
	if (!result) { return false; }

	m_Instancing13->Render();
	result = m_Shader->RenderInstancingShader(m_Instancing13->GetVertexCount(), m_Instancing13->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing13->GetTexture());
	if (!result) { return false; }

	m_Instancing14->Render();
	result = m_Shader->RenderInstancingShader(m_Instancing14->GetVertexCount(), m_Instancing14->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing14->GetTexture());
	if (!result) { return false; }

	Rasterizer::Get()->SetOnCullMode();




	// 빌보딩
	//-----------------------------
	D3DXVECTOR3 TreePosition;
	TreePosition = { 480.0f, 105.0f, 537.0f };

	m_Billboarding->RotationY(CalculateBillboarding(CameraPos, TreePosition));

	m_Billboarding->Multiply(m_Billboarding->GetScailingMatrix(), m_Billboarding->GetRotationYMatrix());
	m_Billboarding->Multiply(m_Billboarding->GetFinalMatrix(), m_Billboarding->GetTranslationMatrix());


	m_Billboarding->Render();
	result = m_Shader->RenderTextureShader(m_Billboarding->GetIndexCount(), m_Billboarding->GetFinalMatrix(), viewMatrix, projectionMatrix, m_Billboarding->GetTexture());
	if (!result) { return false; }

	D3DXVECTOR3 MarkPosition;
	MarkPosition = { 513.0f, 55.0f, 405.0f };

	m_Mark->RotationY(CalculateBillboarding(CameraPos, MarkPosition));

	m_Mark->Multiply(m_Mark->GetScailingMatrix(), m_Mark->GetRotationYMatrix());
	m_Mark->Multiply(m_Mark->GetFinalMatrix(), m_Mark->GetTranslationMatrix());

	m_Mark->Render();
	result = m_Shader->RenderTextureShader(m_Mark->GetIndexCount(), m_Mark->GetFinalMatrix(), viewMatrix, projectionMatrix, m_Mark->GetTexture());
	if (!result) { return false; }

	//-----------------------------

	// 불 빌보딩
	//-----------------------------
	renderFire = m_Frustum->CheckPoint(157.0f, 76.0f, 429.0f);

	if (m_Fire->active == true && m_Fire->active ==true)
	{
		m_Fire->Render();

		result = m_Shader->RenderFireShader(m_Fire->GetIndexCount(), m_Fire->GetFinalMatrix(), viewMatrix,
			projectionMatrix, m_Fire->GetTripleTexture1(), m_Fire->GetTripleTexture2(), m_Fire->GetTripleTexture3(),
			fire_frametime, scrollSpeeds, scales, distortion1, distortion2, distortion3, distortionScale, distortionBias);
		if (!result) { return false; }
	}

	renderTFire = m_Frustum->CheckCube(130.0f, 54.0f, 521.0f, 8.0f);

	if (renderTFire==true && m_TFire->active == true)
	{
		D3DXVECTOR3 TFirePosition = { 130.0f, 56.0f, 521.0f };
		m_TFire->RotationY(CalculateBillboarding(CameraPos, TFirePosition));

		m_TFire->Multiply(m_TFire->GetScailingMatrix(), m_TFire->GetRotationYMatrix());
		m_TFire->Multiply(m_TFire->GetFinalMatrix(), m_TFire->GetTranslationMatrix());

		m_TFire->Render();

		result = m_Shader->RenderFireShader(m_TFire->GetIndexCount(), m_TFire->GetFinalMatrix(), viewMatrix,
			projectionMatrix, m_TFire->GetTripleTexture1(), m_TFire->GetTripleTexture2(), m_TFire->GetTripleTexture3(),
			fire_frametime, scrollSpeeds, scales, distortion1, distortion2, distortion3, distortionScale, distortionBias);
		if (!result) { return false; }
	}
	//260 56 348
	renderTFire2 = m_Frustum->CheckCube(260.0f, 57.0f, 340.0f, 8.0f);

	if (m_TFire2->active == true && renderTFire2==true)
	{

		D3DXVECTOR3 TFire2Position = { 260.0f, 60.0f, 348.0f };
		m_TFire2->RotationY(CalculateBillboarding(CameraPos, TFire2Position));

		m_TFire2->Multiply(m_TFire2->GetScailingMatrix(), m_TFire2->GetRotationYMatrix());
		m_TFire2->Multiply(m_TFire2->GetFinalMatrix(), m_TFire2->GetTranslationMatrix());

		m_TFire2->Render();


		result = m_Shader->RenderFireShader(m_TFire2->GetIndexCount(), m_TFire2->GetFinalMatrix(), viewMatrix,
			projectionMatrix, m_TFire2->GetTripleTexture1(), m_TFire2->GetTripleTexture2(), m_TFire2->GetTripleTexture3(),
			fire_frametime, scrollSpeeds, scales, distortion1, distortion2, distortion3, distortionScale, distortionBias);
		if (!result) { return false; }

	}

	renderTFire3 = m_Frustum->CheckCube(160.0f, 54.0f, 651.0f, 8.0f);

	if (m_TFire3->active == true && renderTFire3 == true)
	{

		D3DXVECTOR3 TFire3Position = { 160.0f, 56.0f, 651.0f };
		m_TFire3->RotationY(CalculateBillboarding(CameraPos, TFire3Position));

		m_TFire3->Multiply(m_TFire3->GetScailingMatrix(), m_TFire3->GetRotationYMatrix());
		m_TFire3->Multiply(m_TFire3->GetFinalMatrix(), m_TFire3->GetTranslationMatrix());

		m_TFire3->Render();

		result = m_Shader->RenderFireShader(m_TFire3->GetIndexCount(), m_TFire3->GetFinalMatrix(), viewMatrix,
			projectionMatrix, m_TFire3->GetTripleTexture1(), m_TFire3->GetTripleTexture2(), m_TFire3->GetTripleTexture3(),
			fire_frametime, scrollSpeeds, scales, distortion1, distortion2, distortion3, distortionScale, distortionBias);
		if (!result) { return false; }
	}

	renderTFire4 = m_Frustum->CheckCube(345.0f, 54.0f, 493.0f, 8.0f);

	if (m_TFire4->active == true && renderTFire4 == true)
	{

		D3DXVECTOR3 TFire4Position = { 345.0f, 56.0f, 493.0f };
		m_TFire4->RotationY(CalculateBillboarding(CameraPos, TFire4Position));

		m_TFire4->Multiply(m_TFire4->GetScailingMatrix(), m_TFire4->GetRotationYMatrix());
		m_TFire4->Multiply(m_TFire4->GetFinalMatrix(), m_TFire4->GetTranslationMatrix());

		m_TFire4->Render();

		result = m_Shader->RenderFireShader(m_TFire4->GetIndexCount(), m_TFire4->GetFinalMatrix(), viewMatrix,
			projectionMatrix, m_TFire4->GetTripleTexture1(), m_TFire4->GetTripleTexture2(), m_TFire4->GetTripleTexture3(),
			fire_frametime, scrollSpeeds, scales, distortion1, distortion2, distortion3, distortionScale, distortionBias);
		if (!result) { return false; }
	}

	renderBFire = m_Frustum->CheckCube(250.0f, 51.0f, 568.5f, 8.0f);

	if (m_BFire->active == true && renderBFire == true)
	{

		D3DXVECTOR3 BFirePosition = { 250.0f, 53.0f, 568.5f };
		m_BFire->RotationY(CalculateBillboarding(CameraPos, BFirePosition));

		m_BFire->Multiply(m_BFire->GetScailingMatrix(), m_BFire->GetRotationYMatrix());
		m_BFire->Multiply(m_BFire->GetFinalMatrix(), m_BFire->GetTranslationMatrix());

		m_BFire->Render();

		result = m_Shader->RenderFireShader(m_BFire->GetIndexCount(), m_BFire->GetFinalMatrix(), viewMatrix,
			projectionMatrix, m_BFire->GetTripleTexture1(), m_BFire->GetTripleTexture2(), m_BFire->GetTripleTexture3(),
			fire_frametime, scrollSpeeds, scales, distortion1, distortion2, distortion3, distortionScale, distortionBias);
		if (!result) { return false; }
	}

	// 2D
	//-------------------------------------------------------------------------------------
	DepthStencil::Get()->SetOffState();
	//Blender::Get()->SetLinear();


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

	//UI
	m_UIManager->renderUI(m_Shader->m_TextureShader, worldMatrix, ImageViewMatrix, orthoMatrix);

	//물풍선UI
	if (m_Balloon->active == true)
	{
		if (Balloon1 == true)
		{
			m_Balloon->Render((width / 2) - 200.0f, 25.0f);
			m_Shader->RenderTextureShader(m_Balloon->GetIndexCount(), worldMatrix, ImageViewMatrix, orthoMatrix, m_Balloon->GetTexture());
			if (!result) { return false; }
		}

		if (Balloon2 == true)
		{
			m_Balloon->Render((width / 2) - 100.0f, 25.0f);
			m_Shader->RenderTextureShader(m_Balloon->GetIndexCount(), worldMatrix, ImageViewMatrix, orthoMatrix, m_Balloon->GetTexture());
			if (!result) { return false; }
		}

		if (Balloon3 == true)
		{
			m_Balloon->Render((width / 2), 25.0f);
			m_Shader->RenderTextureShader(m_Balloon->GetIndexCount(), worldMatrix, ImageViewMatrix, orthoMatrix, m_Balloon->GetTexture());
			if (!result) { return false; }
		}
	}

	// Text
	//------------------------------------------------
	result = m_Text->SetMousePosition(MousePosX, MousePosY);
	result = m_Text->SetPosition(CameraPos.x, CameraPos.y, CameraPos.z);
	result = m_Text->Render(worldMatrix, orthoMatrix);

	Blender::Get()->SetOff();
	DepthStencil::Get()->SetOnState();
	//-------------------------------------------------------------------------------------

	// 파티클
	//-------------------------------------------------------------------------------------

	if (m_Particle->active == true)
	{
		D3DXVECTOR3 ParticlePosition;

		if (Fparticle == true)
			ParticlePosition = { 157.0f, 22.0f, 429.0f };

		if (TFparticle == true)
			ParticlePosition = { 135.0f, 22.0f, 519.0f };

		if (TF2particle == true)
			ParticlePosition = { 265.0f, 22.0f, 350.0f };

		if (TF3particle == true)
			ParticlePosition = { 160.0f, 22.0f, 651.0f };

		if (TF4particle == true)
			ParticlePosition = { 345.0f, 22.0f, 496.0f };

		if (BFparticle == true)
			ParticlePosition = { 247.0f, 22.0f, 568.5f };
		
		m_Particle->Translation(ParticlePosition.x, ParticlePosition.y, ParticlePosition.z);
		m_Particle->RotationY(CalculateBillboarding(CameraPos, ParticlePosition));
		m_Particle->Multiply(m_Particle->GetRotationYMatrix(), m_Particle->GetTranslationMatrix());

		Blender::Get()->SetAdd();

		m_Particle->Render();

		result = m_Shader->RenderParticleShader(m_Particle->GetIndexCount(), m_Particle->GetFinalMatrix(), viewMatrix, projectionMatrix, m_Particle->GetTexture());
		if (!result) { return false; }

		Blender::Get()->SetOff();

	}
	//-------------------------------------------------------------------------------------


	//버퍼에 그려진 씬을 화면에 표시한다.
	D3D::Get()->EndScene();
	
	return true;
}
//-------------------------------------------------------------------------------------
//이름: CheckIntersection()
//용도: 마우스 피킹 이벤트.
//-------------------------------------------------------------------------------------
void GraphicsClass::CheckIntersection(int mouseX, int mouseY, int m_screenWidth, int m_screenHeight)
{
	n_click++;

	if (n_click ==1)
	{
		if (m_NpcBox->TestIntersection(mouseX, mouseY, m_screenWidth, m_screenHeight, projectionMatrix, viewMatrix, worldMatrix, CameraPos))
			m_UIManager->active();
		else 
			m_UIManager->unactive();

		if (m_circle->TestIntersection(mouseX, mouseY, m_screenWidth, m_screenHeight, projectionMatrix, viewMatrix, worldMatrix, CameraPos))
		{
			m_circle->active = true;
			m_Timer->Start();
			m_Balloon->active = true;
		}

		if (m_WaterBox->TestIntersection(mouseX, mouseY, m_screenWidth, m_screenHeight, projectionMatrix, viewMatrix, worldMatrix, CameraPos) == true && m_Balloon->active == true)
		{
			if (Shoot0)
				Balloon1 = true;

			else if (Shoot1)
				Balloon2 = true;

			else if (Shoot2)
				Balloon3 = true;
		}
	}
	

}
//-------------------------------------------------------------------------------------
//이름: RenderShadowToTexture()
//용도: 그림자를 RTT한다.
//-------------------------------------------------------------------------------------
//bool GraphicsClass::RenderShadowToTexture(bool Pressed)
//{
//	bool result;
//	//렌더링 대상을 렌더링에 맞게 설정한다.
//	m_ShadowMap->SetRenderTarget();
//
//	//렌더링을 텍스처에 지웁니다.
//	m_ShadowMap->ClearRenderTarget(0.0f, 0.0f, 0.0f, 1.0f);
//
//	m_circle->Render();
//	result = m_Shader->RenderDepthShader(m_circle->GetIndexCount(), m_circle->GetFinalMatrix(), lightViewMatrix, lightProjectionMatrix);
//	if (!result) { return false; }
//
//	m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);
//
//	for (int i = 0; i < m_Terrain->GetCellCount(); i++)
//	{
//		// Put the terrain cell buffers on the pipeline.
//		result = m_Terrain->RenderCell(i, m_Frustum);
//		if (result)
//		{
//			result = m_Shader->RenderDepthShader(m_Terrain->GetCellIndexCount(i), worldMatrix, lightViewMatrix, lightProjectionMatrix);
//			if (!result) { return false; }
//		}
//	}
//	// Reset the render target back to the original back buffer and not the render to texture anymore.
//	DepthStencil::Get()->SetDefaultRenderView();
//
//	// Reset the viewport back to the original.
//	Camera::Get()->ResetViewPort();
//
//	return true;
//}

//-------------------------------------------------------------------------------------
//이름: RenderRefractionToTexture()
//용도: 호수 내부의 모습을 RTT로 굴절(Refraction)한다.
//-------------------------------------------------------------------------------------
bool GraphicsClass::RenderRefractionToTexture()
{
	D3DXVECTOR4 clipPlane;

	bool result;

	// Setup a clipping plane based on the height of the water to clip everything above it to create a refraction.
	clipPlane = D3DXVECTOR4(0.0f, -1.0f, 0.0f, m_Water->GetWaterHeight() + 0.1f);

	// Set the render target to be the refraction render to texture.
	m_RefractionTexture->SetRenderTarget();

	// Clear the refraction render to texture.
	m_RefractionTexture->ClearRenderTarget( 0.0f, 0.0f, 0.0f, 1.0f);

	m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);

	for (int i = 0; i < m_Terrain->GetCellCount(); i++)
	{
		// Put the terrain cell buffers on the pipeline.
		result = m_Terrain->RenderCell(i, m_Frustum);
		if (result)
		{

			result = m_Shader->RenderWaterReflectionShader(m_Terrain->GetCellIndexCount(i), worldMatrix, viewMatrix, projectionMatrix,
				m_Terrain->GetColorTexture(), m_Terrain->GetNormalMapTexture(), m_Light->GetDiffuseColor(),
				m_Light->GetDirection(), 2.0f, clipPlane);

			if (!result) { return false; }
		}
	}



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
bool GraphicsClass::RenderMainScene(int mouseX, int mouseY)
{
	bool result;
	D3DXMATRIX MainSceneworldMatrix, orthoMatrix;
	Camera::Get()->GetOrtho(&orthoMatrix);
	D3DXMatrixIdentity(&MainSceneworldMatrix);

	D3D::Get()->BeginScene();

	Blender::Get()->SetLinear();
	DepthStencil::Get()->SetOffState();



	result = m_Start->Render(0, 0);
	if (!result) { return false; }
	
	result = m_Shader->RenderTextureShader(m_Start->GetIndexCount(), MainSceneworldMatrix, ImageViewMatrix, orthoMatrix, m_Start->GetTexture());
	if (!result) { return false; }

	
	result = m_MouseCursor->Render(mouseX, mouseY);
	if (!result) { return false; }
	

	D3DXMatrixMultiply(&MainSceneworldMatrix, &MainSceneworldMatrix, &m_MouseCursor->GetScailingMatrix());

	result = m_Shader->RenderTextureShader(m_MouseCursor->GetIndexCount(), MainSceneworldMatrix, ImageViewMatrix, orthoMatrix, m_MouseCursor->GetTexture());
	if (!result) { return false; }
	DepthStencil::Get()->SetOnState();

	D3D::Get()->EndScene();

	return true;
}

/*----------------------------------------------------------------------------------------
이름 : CalculateBillboarding()
용도 : 빌보딩 계산 함수.
------------------------------------------------------------------------------------------*/
float GraphicsClass::CalculateBillboarding(D3DXVECTOR3 cameraPosition, D3DXVECTOR3 BillboardPosition)
{
	float angle;
	float rotation;
	angle = atan2(BillboardPosition.x - cameraPosition.x, BillboardPosition.z - cameraPosition.z) * (180.0 / D3DX_PI);
	rotation = (float)angle * 0.0174532925f;

	return rotation;
}

/*----------------------------------------------------------------------------------------
이름 : SetEffectVariable
용도 : Effect변수 계산 함수
------------------------------------------------------------------------------------------*/
void GraphicsClass::SetEffectVariable(float frametime)
{
	// 불꽃 이펙트
	//-------------------------------------------------------------------------------------
	fire_frametime += frametime;
	if (fire_frametime > 1000.0f) { fire_frametime = 0.0f; }

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

/*----------------------------------------------------------------------------------------
이름 : RenderEndScene()
용도 : 엔딩화면을 그린다.
------------------------------------------------------------------------------------------*/
bool GraphicsClass::RenderEndScene()
{
	bool result;
	D3DXMATRIX MainSceneworldMatrix, orthoMatrix;
	Camera::Get()->GetOrtho(&orthoMatrix);
	D3DXMatrixIdentity(&MainSceneworldMatrix);

	D3D::Get()->BeginScene();

	Blender::Get()->SetLinear();
	DepthStencil::Get()->SetOffState();


	result = m_Ending->Render(0, 0);
	if (!result) { return false; }

	result = m_Shader->RenderTextureShader(m_Ending->GetIndexCount(), MainSceneworldMatrix, ImageViewMatrix, orthoMatrix, m_Ending->GetTexture());
	if (!result) { return false; }

	m_Text->EndRender(MainSceneworldMatrix, orthoMatrix);
	m_Text->SetEndTime(m_endingTime);

	D3D::Get()->EndScene();

	return true;
}

bool GraphicsClass::End()
{
	if (OffFire == true && OffTFire == true && OffTFire2 == true && OffTFire3 ==true && OffTFire4 ==true&& OffBFire==true&&m_UIManager->m_UI[0]->active==true)
	{
		PreShutdown();
		isEnd = true;
	}

	return isEnd;
}