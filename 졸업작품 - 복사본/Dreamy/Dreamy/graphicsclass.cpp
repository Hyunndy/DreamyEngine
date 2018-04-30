////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"




GraphicsClass::GraphicsClass()
{
	dz = 0.0f;
	dy = 0.0f;
}

GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}
GraphicsClass::~GraphicsClass()
{
}


/*----------------------------------------------------------------------------------------
�̸� : Initialize()
�뵵 : D3D��ü, ī�޶�, ����ȭ�� ��ü �ʱ�ȭ
------------------------------------------------------------------------------------------*/
bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;


	// D3D��ü ����
	//-------------------------------------------------------------------------------------
	D3DInfo info;
	D3D::GetInfo(&info);
	D3D::Get();

	width = info.screenWidth;
	height = info.screenHeight;
	// Shader��ü ����
	//-------------------------------------------------------------------------------------
	m_Shader = new ShaderManagerClass;
	if (!m_Shader) { return false; }
	
	result = m_Shader->Initialize(hwnd);
	if (!result){MessageBox(hwnd, L"Could not initialize  shader object.", L"Error", MB_OK); return false;}
	//-------------------------------------------------------------------------------------

	// Camera��ü ����
	//-------------------------------------------------------------------------------------
	Camera::Get()->SetPosition(D3DXVECTOR3(0.0f, 0.0f, -1.0f));
	Camera::Get()->GetView(&ImageViewMatrix);
	Camera::Delete();
	Camera::Get()->SetPosition(D3DXVECTOR3(0.0f, 3.0f, -10.0f));
	Camera::Get()->GetView(&cupViewMatrix);
	Camera::Delete();
	//-------------------------------------------------------------------------------------

	// �����̹��� ����
	//-------------------------------------------------------------------------------------
	m_Start = new ImageClass;
	if (!m_Start) { return false; }

	result = m_Start->Initialize(screenWidth, screenHeight, L"../Dreamy/Data/Main.jpg", screenWidth, screenHeight);
	if (!result) { MessageBox(hwnd, L"Could not initialize the main object.", L"Error", MB_OK); return false; }
	//-------------------------------------------------------------------------------------



	return true;
}


/*----------------------------------------------------------------------------------------
�̸� : Loading()
�뵵 : ������ ������ �Ǵ� �ֵ� �ʱ�ȭ
------------------------------------------------------------------------------------------*/
bool GraphicsClass::Loading(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	// �ε��̹��� ��ü ���� �� ���
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
	
	wstring tPosePath = filePath + L"Horse.fbx";
	wstring idlePath = filePath + L"Horse_Idle.fbx";
	wstring runPath = filePath + L"Horse_Run.fbx";
	//wstring walkPath = filePath + L"Horse_Walk.fbx";
	//horsePos = { 333.0f, 10.0f, 349.0f };
	//horse2Pos = { 235.0f, 10.0f, 500.0f };

	//��1
	m_horse = new ModelScene();

	m_horse->TexturePath = filePath + L"Horse_D.png";	
	m_horse->LoadScene(tPosePath, true, true, true, false);
	m_horse->LoadScene(runPath, false, false, false, true);
	
	// Default Animation ����
	m_horse->SetCurrentAnimation(runPath);


	m_horse->Scale(1.8f, 1.8f, 1.8f);


	//��2
	m_horse2 = new ModelScene();

	m_horse2->TexturePath = m_horse->TexturePath;
	m_horse2->LoadScene(tPosePath, true, true, true, false);
	m_horse2->LoadScene(runPath, false, false, false, true);

	m_horse2->SetCurrentAnimation(runPath);

	m_horse2->Scale(1.8f, 1.8f, 1.8f);

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

	//wstring tPosePath2 = filePath + L"Elf.fbx";
	//m_npc->TexturePath = filePath + L"Elf_D.png";
	//
	//wstring idlePath2 = filePath + L"Elf_Idle.fbx";
	//
	//m_npc->LoadScene(tPosePath2, true, true, true, false);
	//m_npc->LoadScene(idlePath2, false, false, false, true);
	//
	//// Default Animation ����
	//m_npc->SetCurrentAnimation(idlePath2);
	//
	//D3DXMATRIX NpcWorldMatrix;
	//D3DXMATRIX NpcTranslationMatrix;
	//D3DXMATRIX NpcRotationMatrix;
	//
	//D3DXMatrixIdentity(&NpcWorldMatrix);
	//D3DXMatrixIdentity(&NpcRotationMatrix);
	//D3DXMatrixIdentity(&NpcTranslationMatrix);
	//D3DXMatrixRotationY(&NpcRotationMatrix, -35.0f);
	//D3DXMatrixTranslation(&NpcTranslationMatrix, 545.0f, 10.0f, 345.0f);
	//D3DXMatrixMultiply(&NpcWorldMatrix, &NpcRotationMatrix, &NpcTranslationMatrix);
	//m_npc->SetWorldTransform(NpcWorldMatrix);




	//--------------------------------------------------------------------------------------

	// OBJ��
	//--------------------------------------------------------------------------------------
	
	//ť��(���� npc���� �پ�����)
	m_cube = new ModelClass;
	if (!m_cube) { return false; }
	
	result = m_cube->Initialize("../Dreamy/Data/cube.txt", L"../Dreamy/Data/water.png");
	if (!result) { MessageBox(hwnd, L"Could not m_cube object", L"Error", MB_OK); return false; }

	m_cube->Translation(513.0f, 30.0f, 410.0f);
	//(513.0f, 55.0f, 405.0f
	m_cube->Scale(1.5f, 1.5f, 1.5f);

	m_cube->Multiply(m_cube->GetScailingMatrix(), m_cube->GetTranslationMatrix());

	m_wcube = new ModelClass;
	if (!m_wcube) { return false; }

	result = m_wcube->Initialize("../Dreamy/Data/cube.txt", L"../Dreamy/Data/TreeTexture.jpg");
	if (!result) { MessageBox(hwnd, L"Could not m_cube object", L"Error", MB_OK); return false; }

	m_wcube->Translation(766.0f, 11.0f, 470.0f);
	m_wcube->Scale(10.0f, 1.0f, 10.0f);

	m_wcube->Multiply(m_wcube->GetScailingMatrix(), m_wcube->GetTranslationMatrix());
	
	//��ǳ��
	m_circle = new ModelClass;
	if (!m_circle) { return false; }

	result = m_circle->Initialize("../Dreamy/Data/sphere.txt", L"../Dreamy/Data/water.png");
	if (!result) { MessageBox(hwnd, L"Could not m_cube object", L"Error", MB_OK); return false; }

	m_circle->Translation(685.0f, 20.0f, 415.0f);
	m_circle->Scale(3.0f,3.0f, 3.0f);
	m_circle->Multiply(m_circle->GetScailingMatrix(), m_circle->GetTranslationMatrix());
	
	D3DXMatrixIdentity(&Cube3WorldMatrix);

	//��
	m_House = new ModelClass;

	result = m_House->Initialize("../Dreamy/Data/BakerHouse.txt", L"../Dreamy/Data/BakerHouse.png");
	//result = m_House->Initialize("../Dreamy/Data/House2.txt", L"../Dreamy/Data/BakerHouse.png");
	if (!result) { MessageBox(hwnd, L"Could not initialize tree object", L"Error", MB_OK); return false; }

	m_House->Translation(70.0f, 8.0f, 730.0f);
	m_House->RotationY(-30.0f);
	m_House->Multiply(m_House->GetRotationYMatrix(), m_House->GetTranslationMatrix());
	

	
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
	m_Light->SetSpecularPower(16.0f); // �ݻ簭������ �������� �ݻ籤 ȿ���� Ŀ����!
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

	result = m_Sky->InitializeCloud( L"../Dreamy/Data/cloud002.dds");
	if (!result) { MessageBox(hwnd, L"Could not initialize Cloud object", L"Error", MB_OK); return false; }

	//--------------------------------------------------------------------------------------


	// ȣ��
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


	D3DXMatrixTranslation(&WaterWorldMatrix, 766.0f, m_Water->GetWaterHeight(), 470.0f);
	D3DXMatrixRotationY(&WaterRotationMatrix, 70.0f);
	D3DXMatrixMultiply(&WaterWorldMatrix, &WaterRotationMatrix, &WaterWorldMatrix);
	//-------------------------------------------------------------------------------------

	// Text
	//--------------------------------------------------------------------------------------
	m_Text = new TextClass;
	if (!m_Text) { return false; }

	result = m_Text->Initialize(hwnd, screenWidth, screenHeight, ImageViewMatrix);
	if (!result) { MessageBox(hwnd, L"m_Text error", L"Error", MB_OK); return false; }
	//--------------------------------------------------------------------------------------

	// 2D�̹���
	//--------------------------------------------------------------------------------------
	m_CrossHair = new ImageClass;
	if (!m_CrossHair) { return false; }
	
    result = m_CrossHair->Initialize(screenWidth, screenHeight, L"../Dreamy/Data/sibal.png", screenWidth, screenHeight);
	if (!result) { MessageBox(hwnd, L"Could not initialize Crosshair", L"Error", MB_OK); return false; }

	m_MouseCursor = new ImageClass;
	if (!m_MouseCursor) { return false; }

	result = m_MouseCursor->Initialize(screenWidth, screenHeight, L"../Dreamy/Data/mouse.dds", 32, 32);
	if (!result) { MessageBox(hwnd, L"m_MouseCursor error", L"Error", MB_OK); return false; }

	//m_UI = new ImageClass;
	//
	//result = m_UI->Initialize(screenWidth, screenHeight, L"../Dreamy/Data/UI4.png", screenWidth, screenHeight);
	//if (!result) { MessageBox(hwnd, L"UI error", L"Error", MB_OK); return false; }

	m_UIManager = new UIManagerClass;

	m_UIManager->Initialize(screenWidth, screenHeight);

	m_Balloon = new ImageClass;

	result = m_Balloon->Initialize(screenWidth, screenHeight, L"../Dreamy/Data/Balloon.png", 88, 85);
	if (!result) { MessageBox(hwnd, L"ballon error", L"Error", MB_OK); return false; }
	//--------------------------------------------------------------------------------------

	// �ν��Ͻ� ��ü�� ����
	//--------------------------------------------------------------------------------------
	m_Instancing = new InstancingClass;
	if (!m_Instancing) { return false; }

	m_Instancing->SetInstanceCount(6);
	m_Instancing->SetInsatanceVariable(-5.0f, 0.0f, 25.0f);
	m_Instancing->SetInstancePosition(140.0f, 11.0f, 470.0f);

	result = m_Instancing->Initialize( "../Dreamy/Data/MapleTreeStem.txt", L"../Dreamy/Data/bark_0021.jpg");
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

	result = m_Instancing3->Initialize("../Dreamy/Data/MapleTreeStem.txt", L"../Dreamy/Data/bark_0021.jpg");
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

	result = m_Instancing5->Initialize("../Dreamy/Data/MapleTreeStem.txt", L"../Dreamy/Data/bark_0021.jpg");
	if (!result) { MessageBox(hwnd, L"instancing", L"Error", MB_OK); return false; }

	m_Instancing6 = new InstancingClass;
	if (!m_Instancing6) { return false; }
	//124 340
	m_Instancing6->SetInstanceCount(7);
	m_Instancing6->SetInsatanceVariable(-25.0f, 0.0f, 0.0f);
	m_Instancing6->SetInstancePosition(285.0f, 10.0f, 600.0f);

	result = m_Instancing6->Initialize("../Dreamy/Data/MapleTreeLeaves.txt", L"../Dreamy/Data/green_leaf.png");
	if (!result) { MessageBox(hwnd, L"instancing", L"Error", MB_OK); return false; }

	//744 51 554 x��Ŀ����
	m_Instancing7 = new InstancingClass;
	if (!m_Instancing7) { return false; }

	m_Instancing7->SetInstanceCount(5);
	m_Instancing7->SetInsatanceVariable(25.0f, 0.0f, 0.0f);
	m_Instancing7->SetInstancePosition(735.0f, 13.0f, 530.0f);

	result = m_Instancing7->Initialize("../Dreamy/Data/MapleTreeStem.txt", L"../Dreamy/Data/bark_0021.jpg");
	if (!result) { MessageBox(hwnd, L"instancing", L"Error", MB_OK); return false; }

	m_Instancing8 = new InstancingClass;
	if (!m_Instancing8) { return false; }

	m_Instancing8->SetInstanceCount(5);
	m_Instancing8->SetInsatanceVariable(25.0f, 0.0f, 0.0f);
	m_Instancing8->SetInstancePosition(735.0f, 13.0f, 530.0f);

	result = m_Instancing8->Initialize("../Dreamy/Data/MapleTreeLeaves.txt", L"../Dreamy/Data/maple_leaf.png");
	if (!result) { MessageBox(hwnd, L"instancing", L"Error", MB_OK); return false; }

	//m_Instancing9 = new InstancingClass;
	//if (!m_Instancing9) { return false; }
	//
	//m_Instancing9->SetInstanceCount(9);
	//m_Instancing9->SetInsatanceVariable(20.0f, 0.0f, 0.0f);
	//m_Instancing9->SetInstancePosition(722.0f, 13.0f, 548.0f);
	//
	//result = m_Instancing9->Initialize("../Dreamy/Data/MapleTreeStem.txt", L"../Dreamy/Data/bark_0021.jpg");
	//if (!result) { MessageBox(hwnd, L"instancing", L"Error", MB_OK); return false; }
	//
	//
	//m_Instancing10 = new InstancingClass;
	//if (!m_Instancing10) { return false; }
	//
	//m_Instancing10->SetInstanceCount(9);
	//m_Instancing10->SetInsatanceVariable(20.0f, 0.0f, 0.0f);
	//m_Instancing10->SetInstancePosition(722.0f, 13.0f, 548.0f);
	//
	//result = m_Instancing10->Initialize("../Dreamy/Data/MapleTreeLeaves.txt", L"../Dreamy/Data/maple_leaf.png");
	//if (!result) { MessageBox(hwnd, L"instancing", L"Error", MB_OK); return false; }

	//--------------------------------------------------------------------------------------

	// �̴ϸ�
	//--------------------------------------------------------------------------------------
	m_Minimap = new MinimapClass;
	if (!m_Minimap) { return false; }

	result = m_Minimap->Initialize( screenWidth, screenHeight, 1025, 1025);
	if (!result) { MessageBox(hwnd, L"minimap error", L"Error", MB_OK); return false; }
	//--------------------------------------------------------------------------------------

	// ������ ��ü(��ƼŬ ��)
	//--------------------------------------------------------------------------------------
	m_Particle = new ParticleSystem;
	if (!m_Particle) { return false; }

	result = m_Particle->Initialize( L"../Dreamy/Data/star.dds");
	if (!result) { MessageBox(hwnd, L"Could not particle TitleText", L"Error", MB_OK); return false; }

	//������
	m_Billboarding = new ModelClass;

	result = m_Billboarding->Initialize("../Dreamy/Data/square2.txt", L"../Dreamy/Data/Billboard.png");
	if (!result) { MessageBox(hwnd, L"Could not initialize tree object", L"Error", MB_OK); return false; }

	m_Billboarding->Translation(480.0f, 105.0f, 537.0f);
	m_Billboarding->Scale(1.5f, 1.5f, 1.5f);

	//��ũ
	m_Mark = new ModelClass;

	result = m_Mark->Initialize("../Dreamy/Data/square2.txt", L"../Dreamy/Data/Mark.png");
	if (!result) { MessageBox(hwnd, L"Could not initialize tree object", L"Error", MB_OK); return false; }

	m_Mark->Translation(513.0f, 55.0f, 405.0f);
	m_Mark->Scale(0.4f, 0.4f, 0.4f);


	//���� �� ��
	m_Fire = new ModelClass;

	result = m_Fire->InitializeTriple("../Dreamy/Data/square.txt", L"../Dreamy/Data/fire01.dds", L"../Dreamy/Data/noise01.dds", L"../Dreamy/Data/alpha01.dds");
	if (!result) { MessageBox(hwnd, L"Could not initialize fire effect object", L"Error", MB_OK); return false; }

	m_Fire->Translation(157.0f, 49.0f, 429.0f);
	//m_Fire->RotationY(CalculateBillboarding(CameraPos, firePosition));
	m_Fire->RotationY(29.7f);
	m_Fire->Scale(40.0f, 50.0f, 40.0f);
	m_Fire->Multiply(m_Fire->GetScailingMatrix(), m_Fire->GetRotationYMatrix());
	m_Fire->Multiply(m_Fire->GetFinalMatrix(), m_Fire->GetTranslationMatrix());
	m_Fire->active = true;

	//����1�� �� ��
	m_TFire = new ModelClass;

	result = m_TFire->InitializeTriple("../Dreamy/Data/square.txt", L"../Dreamy/Data/fire01.dds", L"../Dreamy/Data/noise01.dds", L"../Dreamy/Data/alpha01.dds");
	if (!result) { MessageBox(hwnd, L"Could not initialize m_TFire effect object", L"Error", MB_OK); return false; }

	m_TFire->Translation(130.0f, 53.0f, 521.0f);
	m_TFire->Scale(20.0f, 20.0f, 20.0f);
	m_TFire->active = true;

	//����3�� �� ��
	m_TFire2 = new ModelClass;

	result = m_TFire2->InitializeTriple("../Dreamy/Data/square.txt", L"../Dreamy/Data/fire01.dds", L"../Dreamy/Data/noise01.dds", L"../Dreamy/Data/alpha01.dds");
	if (!result) { MessageBox(hwnd, L"Could not initialize m_TFire2 effect object", L"Error", MB_OK); return false; }

	m_TFire2->Translation(260.0f, 56.0f, 348.0f);
	m_TFire2->Scale(20.0f, 20.0f, 20.0f);
	m_TFire2->active = true;
	//--------------------------------------------------------------------------------------
	
	return true;
}

// ��� �׷��� ��ü�� ���ᰡ ���⿡�� �Ͼ��.
void GraphicsClass::PreShutdown()
{
	FBXShader::DeleteBuffer();
	SAFE_SHUTDOWN(m_UI);
	SAFE_SHUTDOWN(m_UIManager);
	SAFE_SHUTDOWN(m_Minimap);
	SAFE_DELETE(m_horse);
	SAFE_DELETE(m_horse2);
	SAFE_DELETE(m_npc);
	SAFE_SHUTDOWN(m_Billboarding);
	SAFE_SHUTDOWN(m_Mark);
	SAFE_DELETE(m_Light);
	SAFE_SHUTDOWN(m_Fire);
	SAFE_SHUTDOWN(m_TFire);
	SAFE_SHUTDOWN(m_TFire2);
	SAFE_SHUTDOWN(m_circle);
	SAFE_SHUTDOWN(m_TerrainShader);
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
	SAFE_SHUTDOWN(m_Terrain);
	SAFE_SHUTDOWN(m_House);
	SAFE_SHUTDOWN(m_Sky);
	SAFE_SHUTDOWN(m_cube);
	SAFE_SHUTDOWN(m_wcube);
	SAFE_SHUTDOWN(m_CrossHair);
	SAFE_DELETE(m_Frustum);
	SAFE_SHUTDOWN(m_MouseCursor);
	SAFE_SHUTDOWN(m_Text);
	SAFE_SHUTDOWN(m_Particle);

}

void GraphicsClass::Shutdown()
{
	SAFE_SHUTDOWN(m_Loading);
	Rasterizer::Delete();
	Blender::Delete();
	DepthStencil::Delete();
	Camera::Delete();
	D3D::Delete();
}

/*----------------------------------------------------------------------------------------
�̸� : Frame()
�뵵 : 
- SystemClass���� ������ ���� ȣ��ȴ�.
- ���콺������ �����̴°� �� �� ����
- fps,cpu,frametime�缭 text�� ���.
- ��ü ���� �� ����
- �� ������ ���� ������ ��ȯ.
- �� ������ ���� ȣ���� ��ȯ�� ȣ�� ǥ�� �ؽ�ó�� 3�ܰ� RTT <- �̰� �ʹ� ��� �����.
------------------------------------------------------------------------------------------*/
// �� ȣ�⸶�� Render�Լ��� �θ���.
bool GraphicsClass::Frame(int fps,  float frameTime, D3DXVECTOR3 pos, D3DXVECTOR3 rot, int mouseX, int mouseY, bool space)
{
	bool result;
	bool foundHeight;

	if (click == false)
		cibal = 0;
	
	// Camera Frameó��
	//--------------------------------------------------------------------------------------

	float CameraY;
	Camera::Get()->GetPosition(&CameraPos);
	
	foundHeight = m_Terrain->GetHeightAtPosition(CameraPos.x, CameraPos.z, CameraY);
	if (foundHeight)
		CameraPos.y = CameraY + 40.0f;

	Camera::Get()->SetPosition(CameraPos);
	Camera::Get()->Update();
	//--------------------------------------------------------------------------------------

	// ���콺 
	//--------------------------------------------------------------------------------------
	MousePosX = mouseX;
	MousePosY = mouseY;
	//--------------------------------------------------------------------------------------

	// �ؽ�Ʈ Frameó��
	//--------------------------------------------------------------------------------------
	// fps����
	result = m_Text->SetFps(m_Particle->active);
	if (!result) { return false; }

	//// cpu����
	//result = m_Text->SetCpu(cpu);
	//if (!result) { return false; }
	//--------------------------------------------------------------------------------------

	// ��ǳ��
	//--------------------------------------------------------------------------------------
	if (space == true)
		shoot = true;

	if (shoot == true && m_circle->active == true && (Balloon1 == true || Balloon2 == true || Balloon3 == true))
		Shoot(frameTime);
	//--------------------------------------------------------------------------------------

	// ��ǳ�� ó�� 130.0f, 53.0f, 521.0f
	//--------------------------------------------------------------------------------------
	if (shoot == true && m_circle->active == true && m_TFire->active == true && CameraPos.x <= 180.0f &&CameraPos.z >= 490.0f&& CameraPos.z <= 560.0f)
	{
		if (dx == 129)
		{
			EffectSound = true;
			m_Particle->active = true;
			TFparticle = true;
			m_TFire->active = false;
			OffTFire = true;
		}
	}

	//230 280
	if (shoot == true && m_circle->active == true && m_TFire2->active == true && CameraPos.z <= 385.0f && CameraPos.x >=230.0f && CameraPos.x <=280.0f)
	{
		if (dx == 129)
		{
			EffectSound = true;
			m_Particle->active = true;
			TF2particle = true;
			m_TFire2->active = false;
			OffTFire2 = true;

		}

	}
	// 157.0f, 49.0f, 429.0f
	if (shoot == true && m_circle->active == true && m_Fire->active == true && CameraPos.x <= 215.0f && CameraPos.z >=385.0f && CameraPos.z <=465.0f)
	{
		if (dx == 129)
		{
			EffectSound = true;
			m_Particle->active = true;
			Fparticle = true;
			m_Fire->active = false;
			OffFire = true;

		}

	}
	//--------------------------------------------------------------------------------------


	 // Model�� Frameó��
	 // ������ ó�� �޸𸮿� ����Ÿ�������ϱ� ���� ��Ƽ�꿡��������!!!!!
	 //--------------------------------------------------------------------------------------
	m_Sky->Frame(frameTime*0.00001f, 0.0f, frameTime*0.00002f, 0.0f); //����
	m_Minimap->PositionUpdate(CameraPos.x, CameraPos.z); //�̴ϸ�

	if (m_Fire->active == true || m_TFire->active == true || m_TFire2->active == true)
		SetEffectVariable(frameTime*0.0005f);


	if (m_Water->active == true)
		m_Water->Frame(frameTime);

	// ��ƼŬ
	if (m_Particle->active == true)
		m_Particle->Frame(frameTime);

	else
		Fparticle = TFparticle = TF2particle = false;


	//��1
	if (starthorse == true)
	{
		float horseY;
		if (m_Terrain->GetHeightAtPosition(horsePos.x, horsePos.z, horseY))
			horsePos.y = horseY + 2.0f;

		m_horse->Update(1.0f);
		//800

		if (m_horse->turn == false)
		{
			horsePos.x = horsePos.x + frameTime*0.05f;

		}
		else
		{
			horsePos.x = horsePos.x - frameTime*0.05f;

		}

		if (horsePos.x >= 700.0f)
			m_horse->turn = true;

		if (horsePos.x <= 330.0f)
			m_horse->turn = false;
	}

	//609 120
	//��2
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

	//npc
	if (renderNpc == true)
		m_npc->Update(0.5f);
	//--------------------------------------------------------------------------------------

	return true;
}

void GraphicsClass::Shoot(float frametime)
{

		//dy += frameTime*0.00009f;
		//dz += frameTime*0.0005f;

		Accell = Gravity;
		//5
		//3 �϶� ����γ��� �ӵ� 0.95 0.08

		//vVelocity = vVelocity + Accell *frameTime*0.00008f;
		vVelocity = vVelocity + Accell*frametime*0.00017f;

		vPosition = vPosition + vVelocity *frametime*0.00010f;

		dy = vPosition.y;
		dz = vPosition.z;

		D3DXMatrixTranslation(&ShootMatrix, 0.0f, dy, dz);
		D3DXMatrixMultiply(&Cube3WorldMatrix, &Cube3WorldMatrix, &ShootMatrix);

		dx++;

		if (dx == 130)
		{
			shoot = false;
			dx = 0;
			dy = 0.0f;
			dz = 0.0f;
			Accell = { 0.0f, 0.0f, 0.0f };
			vPosition = { 0.0f, 0.0f, 0.0f };
			vVelocity = { 0.0f,1.5f,0.5f };
			D3DXMatrixTranslation(&ShootMatrix, 0.0f, dy, dz);
			D3DXMatrixIdentity(&Cube3WorldMatrix);

			if (Shoot3)
				Balloon3 = false;
			else if (Shoot2)
				Balloon2 = false;
			else if (Shoot1)
				Balloon1 = false;
			
		}
	
}


/*----------------------------------------------------------------------------------------
�̸� : Render()
�뵵 : ���׾��� RTT���� �׸���.
- �ݻ縦 �� ���� �켱 RTT�� �����ϰ� ���׾��� �ε��ؾ� ȥ�յȴ�.
------------------------------------------------------------------------------------------*/
bool GraphicsClass::Render(bool Pressed)
{
	//�⺻���
	//-------------------------------------------------------------------------------------
	D3DXMatrixIdentity(&worldMatrix);
	Camera::Get()->GetView(&viewMatrix);
	Camera::Get()->GetProjection(&projectionMatrix);
	Camera::Get()->GetOrtho(&orthoMatrix);

	bool result;

	if (m_Water->active == true)
	{
		result = RenderRefractionToTexture(Pressed);
		if (!result) { return false; }
	}
	//-------------------------------------------------------------------------------------
	result = RenderRunningScene(Pressed);
	if (!result) { return false; }
	//-------------------------------------------------------------------------------------
	
	return true;
}

/*----------------------------------------------------------------------------------------
�̸� : RenderRunningScene()
�뵵 :
- 1. ī�޶��� ��ġ�� ���� �� ����� ����� ���� ī�޶��� render�Լ��� ȣ���Ѵ�.
- 2. �� ����� ����� ���纻�� �����´�.
- 3. d3dclass��ü�� ���� ���� ��İ� ���� ����� �����ؿ´�.
- 4. modelclass�� render()�Լ��� ȣ���Ͽ� �׷��� ���������ο� �ﰢ�� ���� �׸����� �Ѵ�.
- 5. ���⼭ �غ��� ������� ���̴��� ȣ���Ͽ� ���̴��� �� ������ ������ ��ġ��Ű�� ���� �� ����� ����Ͽ� �������� �׷�����.
- 6. �ﰢ���� ����ۿ� �׷�����.

- ModelClass, ImageClass, TextClass�� Render()�� ����/�ε��� ���۸� �׷��Ƚ� ���������ο� ���� I/A�ܰ� ������ �Ѵ�.
- �� ���ϱ����� �׸���(����ϴ�)�� ShaderClass�� Render()�̴�!

- �Ȱ� ���� �� ����۸� �Ȱ��� ������ ����°��� �ſ� �߿��� �κ��̴�.
- FogEnd�� �־�������(+�ɼ���) ������� = ��ü�� ��ġ(�������̴����� cameraposition)�� FogENd�� �����������(+) FogFactor�� Ŀ���� �ؽ�ó�� ��������.

�Ȱ��� ...���߿� ... ���̸� ���� �� ���̸ʿ��� ���̴��� ����°� ������.
------------------------------------------------------------------------------------------*/
bool GraphicsClass::RenderRunningScene(bool Pressed)
{
	bool result;
	bool renderModel;

	//�Ȱ� ��ü �ʱ�ȭ
	//fogEnd�� �־������� �����
	//-------------------------------------------------------------------------------------
	float fogColor, fogStart, fogEnd;
	fogColor = 0.0f;
	fogStart = 400.0f;
	fogEnd = 550.0f;
	//-------------------------------------------------------------------------------------



	//����
	//-------------------------------------------------------------------------------------
	D3D::Get()->BeginScene();

	//�þ� ����ü(Frustum)����
	//-------------------------------------------------------------------------------------
	m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);

	//-------------------------------------------------------------------------------------

	// Sky
	//-------------------------------------------------------------------------------------
	D3DXMATRIX SkyworldMatrix;
	D3DXMatrixIdentity(&SkyworldMatrix);
	D3DXMatrixTranslation(&SkyworldMatrix, CameraPos.x, CameraPos.y, CameraPos.z);

	//�ø���� ����
	Rasterizer::Get()->SetOffCullMode();
	//�ĸ���� ����
	DepthStencil::Get()->SetOffState();

	m_Sky->RenderSky();
	m_Shader->RenderSkydomeShader(m_Sky->GetIndexCount(), SkyworldMatrix, viewMatrix, projectionMatrix,
		m_Sky->GetApexColor(), m_Sky->GetCenterColor());

	//�ø�����ѱ�
	Rasterizer::Get()->SetOnCullMode();

	//���ĺ��� ȥ�� ���� ����
	Blender::Get()->SetAdd();

	m_Sky->RenderCloud();

	m_Shader->RenderCloudShader(m_Sky->GetCloudIndexCount(), SkyworldMatrix, viewMatrix, projectionMatrix,
		m_Sky->GetCloudTexture1(), m_Sky->GetTranslation(0), m_Sky->GetTranslation(1),
		m_Sky->GetTranslation(2), m_Sky->GetTranslation(3), m_Sky->GetBrightness());

	//���ĺ��� �������
	Blender::Get()->SetOff();
	//�ĸ���� Ű��
	DepthStencil::Get()->SetOnState();
	//-------------------------------------------------------------------------------------

	// Terrain
	//-------------------------------------------------------------------------------------
	// Terrain���� ���� �� ������ŭ ���� �ε��Ѵ�.
	for (int i = 0; i < m_Terrain->GetCellCount(); i++)
	{
		// Put the terrain cell buffers on the pipeline.
		result = m_Terrain->RenderCell(i, m_Frustum);
		if (result)
		{

			result = m_TerrainShader->Render(m_Terrain->GetCellIndexCount(i), worldMatrix, viewMatrix,
				projectionMatrix, m_Terrain->GetColorTexture(), m_Terrain->GetNormalMapTexture(), m_Light->GetDirection(), m_Light->GetDiffuseColor(), fogStart, fogEnd);
			if (!result) { return false; }

		}
	}
	//-------------------------------------------------------------------------------------

	// ȣ��  512.5f, 30.0f, 310.0f
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

	// FBXANIMATIONModel��
	//-------------------------------------------------------------------------------------

	//npc
	renderNpc = m_Frustum->CheckCube(515.0f, 25.0f, 415.0f, 10.0f);
	if (renderNpc == true)
	{
		m_npc->Render();
	}

	//��1
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

	//��2
	renderhorse2 = m_Frustum->CheckSphere(horse2Pos.x, horse2Pos.y, horse2Pos.z, 3.0f);
	if (renderhorse2) { starthorse2 = true; }

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

	// OBJ��
	//-------------------------------------------------------------------------------------

	// ��ǳ�� 766 450
	//-----------------------------
	//m_circle->active = m_Frustum->CheckSphere(660.0f, 48.0f, 383.0f, 3.0f);

	m_circle->Render();
	if (m_circle->active == false)
	{
		//���̴� ������
		result = m_Shader->RenderLightShader(m_circle->GetIndexCount(), m_circle->GetFinalMatrix(), viewMatrix, projectionMatrix
			, m_circle->GetTexture(),D3DXVECTOR3(0.2,-1,1), D3DXVECTOR4(0.9f, 0.9f, 0.9f, 1.0f), m_Light->GetDiffuseColor()
			, CameraPos, m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
		if (!result) { return false; }
	}
	else
		result = m_Shader->RenderLightShader(m_circle->GetIndexCount(), Cube3WorldMatrix, cupViewMatrix, projectionMatrix
			, m_circle->GetTexture(), D3DXVECTOR3(0.2, -1, 1), D3DXVECTOR4(0.9f, 0.9f, 0.9f, 1.0f), m_Light->GetDiffuseColor(), CameraPos
			, m_Light->GetSpecularColor(), m_Light->GetSpecularPower());


	

	// npc �浹 �ڽ�
	//-----------------------------
	m_cube->active = m_Frustum->CheckPoint(515.0f, 30.0f, 415.0f);
	
	if (m_cube->active== true)
	{

		m_cube->Render();
	}

	// �� �浹 �ڽ�
	//-----------------------------
	if (m_Water->active)
		m_wcube->Render();
	
	
	// ��
	//-----------------------------
	m_House->active = m_Frustum->CheckCube(137.0f, 49.0f, 425.0f, 25.0f);

	if (m_House->active == true)
	{
		m_House->Render();

		if (OffFire == false)
		{
			result = m_Shader->RenderDiffuseShader(m_House->GetIndexCount(), m_House->GetFinalMatrix(), viewMatrix, projectionMatrix, m_House->GetTexture(), D3DXVECTOR3(-0.5f, -1.0f, 0.0f), D3DXVECTOR4(0.75f, 0.75f, 0.75f, 1.0f));
			if (!result) { return false; }
		}
		else
		{
			result = m_Shader->RenderDiffuseShader(m_House->GetIndexCount(), m_House->GetFinalMatrix(), viewMatrix, projectionMatrix, m_House->GetTexture(), D3DXVECTOR3(-0.5f, -1.0f, 0.0f), D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));
			if (!result) { return false; }
		}
	}
	//-------------------------------------------------------------------------------------



	// �ν��Ͻ�, ��������ü
	//-------------------------------------------------------------------------------------

	// �ν��Ͻ�
	//-----------------------------
	Rasterizer::Get()->SetOffCullMode();

	if (OffTFire == false)
	{
		m_Instancing->Render(); 
		result = m_Shader->RenderDiffuseInstancingShader(m_Instancing->GetVertexCount(), m_Instancing->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing->GetTexture(), D3DXVECTOR3(-0.5f, -1.0f, 0.0f), D3DXVECTOR4(0.75f, 0.75f, 0.75f, 1.0f));
		if (!result) { return false; }
		m_Instancing2->Render();
		result = m_Shader->RenderDiffuseInstancingShader(m_Instancing2->GetVertexCount(), m_Instancing2->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing2->GetTexture(), D3DXVECTOR3(-0.5f, -1.0f, 0.0f), D3DXVECTOR4(0.75f, 0.75f, 0.75f, 1.0f));
		if (!result) { return false; }
	}
	else
	{
		Blender::Get()->SetLinear();
		m_Instancing->Render();
		result = m_Shader->RenderInstancingShader(m_Instancing->GetVertexCount(), m_Instancing->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing->GetTexture());
		if (!result) { return false; }
		m_Instancing2->Render();
		result = m_Shader->RenderInstancingShader(m_Instancing2->GetVertexCount(), m_Instancing2->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing2->GetTexture());
		if (!result) { return false; }
		Blender::Get()->SetOff();
	}

	if (OffTFire2 == false)
	{
		m_Instancing3->Render();
		result = m_Shader->RenderDiffuseInstancingShader(m_Instancing3->GetVertexCount(), m_Instancing3->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing3->GetTexture(), D3DXVECTOR3(-0.5f, -1.0f, 0.0f), D3DXVECTOR4(0.75f, 0.75f, 0.75f, 1.0f));
		if (!result) { return false; }
		m_Instancing4->Render();
		result = m_Shader->RenderDiffuseInstancingShader(m_Instancing4->GetVertexCount(), m_Instancing4->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing4->GetTexture(), D3DXVECTOR3(-0.5f, -1.0f, 0.0f), D3DXVECTOR4(0.75f, 0.75f, 0.75f, 1.0f));
		if (!result) { return false; }
	}

	else
	{
		Blender::Get()->SetLinear();
		m_Instancing3->Render();
		result = m_Shader->RenderInstancingShader(m_Instancing3->GetVertexCount(), m_Instancing3->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing3->GetTexture());
		if (!result) { return false; }
		m_Instancing4->Render();
		result = m_Shader->RenderInstancingShader(m_Instancing4->GetVertexCount(), m_Instancing4->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing4->GetTexture());
		if (!result) { return false; }
		Blender::Get()->SetOff();
	}

	Blender::Get()->SetLinear();

	m_Instancing5->Render();

	//result = m_Shader->RenderDiffuseInstancingShader(m_Instancing5->GetVertexCount(), m_Instancing5->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing5->GetTexture(), D3DXVECTOR3(-0.5f, -1.0f, 0.0f), D3DXVECTOR4(0.75f, 0.75f, 0.75f, 1.0f));
	//if (!result) { return false; }
	result = m_Shader->RenderInstancingShader(m_Instancing5->GetVertexCount(), m_Instancing5->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing5->GetTexture());
	if (!result) { return false; }

	m_Instancing6->Render();
	//215 46 510
	//result = m_Shader->RenderDiffuseInstancingShader(m_Instancing6->GetVertexCount(), m_Instancing6->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing6->GetTexture(), D3DXVECTOR3(-0.5f, -1.0f, 0.0f), D3DXVECTOR4(0.75f, 0.75f, 0.75f, 1.0f));
	//if (!result) { return false; }
	result = m_Shader->RenderInstancingShader(m_Instancing6->GetVertexCount(), m_Instancing6->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing6->GetTexture());
	if (!result) { return false; }

	m_Instancing7->Render();

	result = m_Shader->RenderInstancingShader(m_Instancing7->GetVertexCount(), m_Instancing7->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing7->GetTexture());
	if (!result) { return false; }

	m_Instancing8->Render();

	result = m_Shader->RenderInstancingShader(m_Instancing8->GetVertexCount(), m_Instancing8->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing8->GetTexture());
	if (!result) { return false; }

	//m_Instancing9->Render();
	//
	//result = m_Shader->RenderInstancingShader(m_Instancing9->GetVertexCount(), m_Instancing9->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing9->GetTexture());
	//if (!result) { return false; }
	//
	//m_Instancing10->Render();
	//
	//result = m_Shader->RenderInstancingShader(m_Instancing10->GetVertexCount(), m_Instancing10->GetInstanceCount(), worldMatrix, viewMatrix, projectionMatrix, m_Instancing10->GetTexture());
	//if (!result) { return false; }

	Rasterizer::Get()->SetOnCullMode();




	// ���� ������
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

	// �� ������
	//-----------------------------
	renderFire = m_Frustum->CheckPoint(157.0f, 49.0f, 429.0f);

	if (m_Fire->active == true && m_Fire->active ==true)
	{
		m_Fire->Render();

		result = m_Shader->RenderFireShader(m_Fire->GetIndexCount(), m_Fire->GetFinalMatrix(), viewMatrix,
			projectionMatrix, m_Fire->GetTripleTexture1(), m_Fire->GetTripleTexture2(), m_Fire->GetTripleTexture3(),
			fire_frametime, scrollSpeeds, scales, distortion1, distortion2, distortion3, distortionScale, distortionBias);
		if (!result) { return false; }
	}

	renderTFire = m_Frustum->CheckPoint(130.0f, 53.0f, 521.0f);

	if (renderTFire==true && m_TFire->active == true)
	{
		D3DXVECTOR3 TFirePosition = { 130.0f, 53.0f, 521.0f };
		m_TFire->RotationY(CalculateBillboarding(CameraPos, TFirePosition));

		m_TFire->Multiply(m_TFire->GetScailingMatrix(), m_TFire->GetRotationYMatrix());
		m_TFire->Multiply(m_TFire->GetFinalMatrix(), m_TFire->GetTranslationMatrix());

		m_TFire->Render();

		result = m_Shader->RenderFireShader(m_TFire->GetIndexCount(), m_TFire->GetFinalMatrix(), viewMatrix,
			projectionMatrix, m_TFire->GetTripleTexture1(), m_TFire->GetTripleTexture2(), m_TFire->GetTripleTexture3(),
			fire_frametime, scrollSpeeds, scales, distortion1, distortion2, distortion3, distortionScale, distortionBias);
		if (!result) { return false; }
	}

	renderTFire2 = m_Frustum->CheckPoint(260.0f, 56.0f, 348.0f);

	if (m_TFire2->active == true && renderTFire2==true)
	{
		D3DXVECTOR3 TFire2Position = { 260.0f, 56.0f, 348.0f };
		m_TFire2->RotationY(CalculateBillboarding(CameraPos, TFire2Position));

		m_TFire2->Multiply(m_TFire2->GetScailingMatrix(), m_TFire2->GetRotationYMatrix());
		m_TFire2->Multiply(m_TFire2->GetFinalMatrix(), m_TFire2->GetTranslationMatrix());

		m_TFire2->Render();

		result = m_Shader->RenderFireShader(m_TFire2->GetIndexCount(), m_TFire2->GetFinalMatrix(), viewMatrix,
			projectionMatrix, m_TFire2->GetTripleTexture1(), m_TFire2->GetTripleTexture2(), m_TFire2->GetTripleTexture3(),
			fire_frametime, scrollSpeeds, scales, distortion1, distortion2, distortion3, distortionScale, distortionBias);
		if (!result) { return false; }
	}

	// 2D
	//-------------------------------------------------------------------------------------
	DepthStencil::Get()->SetOffState();
	//Blender::Get()->SetLinear();


	// Text
	//------------------------------------------------
	result = m_Text->SetMousePosition(MousePosX, MousePosY);
	result = m_Text->SetPosition(CameraPos.x, CameraPos.y, CameraPos.z);
	result = m_Text->Render(worldMatrix, orthoMatrix);

	// �̹�����
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


	//if (m_UI->active == true)
	//{
	//	m_UI->Render(0, 0);
	//	if (!result) { return false; }
	//	m_Shader->RenderTextureShader(m_UI->GetIndexCount(), worldMatrix, ImageViewMatrix, orthoMatrix, m_UI->GetTexture());
	//	if (!result) { return false; }
	//}

	m_UIManager->renderUI(m_Shader->m_TextureShader, worldMatrix, ImageViewMatrix, orthoMatrix);

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


	Blender::Get()->SetOff();
	DepthStencil::Get()->SetOnState();
	//-------------------------------------------------------------------------------------

	// ��ƼŬ
	//-------------------------------------------------------------------------------------

	if (m_Particle->active == true)
	{
		D3DXVECTOR3 ParticlePosition;

		if (Fparticle == true)
			ParticlePosition = { 157.0f, 22.0f, 429.0f };

		if (TFparticle == true)
			ParticlePosition = { 135.0f, 22.0f, 519.0f };

		if (TF2particle == true)
			ParticlePosition = { 260.0f, 22.0f, 350.0f };

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


	//���ۿ� �׷��� ���� ȭ�鿡 ǥ���Ѵ�.
	D3D::Get()->EndScene();
	
	return true;
}
//-------------------------------------------------------------------------------------
//�̸�: CheckIntersection()
//�뵵: ���콺 ��ŷ �̺�Ʈ.
//-------------------------------------------------------------------------------------
void GraphicsClass::CheckIntersection(int mouseX, int mouseY, int m_screenWidth, int m_screenHeight)
{
	cibal++;

	if (cibal ==1)
	{
		if (m_cube->TestIntersection(mouseX, mouseY, m_screenWidth, m_screenHeight, projectionMatrix, viewMatrix, worldMatrix, CameraPos))
		{
			m_UIManager->active();
			//m_Particle->active = true;
			//m_UI->active = true;
		}
		else 
			m_UIManager->unactive();

		//m_UI->active = false;


		if (m_circle->TestIntersection(mouseX, mouseY, m_screenWidth, m_screenHeight, projectionMatrix, viewMatrix, worldMatrix, CameraPos))
		{
			m_circle->active = true;
			m_Balloon->active = true;
		}


		if (m_wcube->TestIntersection(mouseX, mouseY, m_screenWidth, m_screenHeight, projectionMatrix, viewMatrix, worldMatrix, CameraPos) == true && m_Balloon->active == true)
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
//�̸�: RenderRefractionToTexture()
//�뵵: ȣ�� ������ ����� RTT�� ����(Refraction)�Ѵ�.
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
�̸� : RenderLoadingScene()
�뵵 : �ε�ȭ���� �׸���.
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
�̸� : RenderMainScene()
�뵵 : ����ȭ���� �׸���.
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



float GraphicsClass::CalculateBillboarding(D3DXVECTOR3 cameraPosition, D3DXVECTOR3 BillboardPosition)
{
	float angle;
	float rotation;
	angle = atan2(BillboardPosition.x - cameraPosition.x, BillboardPosition.z - cameraPosition.z) * (180.0 / D3DX_PI);
	rotation = (float)angle * 0.0174532925f;

	return rotation;
}

void GraphicsClass::SetEffectVariable(float frametime)
{
	// �Ҳ� ����Ʈ
	//-------------------------------------------------------------------------------------
	fire_frametime += frametime;
	if (fire_frametime > 1000.0f) { fire_frametime = 0.0f; }

	// �� ������ �ؽ����� ��ũ�� �ӵ��� �����մϴ�.
	scrollSpeeds = D3DXVECTOR3(1.3f, 2.1f, 2.3f);

	// �� ũ�Ⱚ�� ����Ͽ� �� ������ �ٸ� ������ ��Ÿ�� �ؽ��ĸ� ����ϴ�.
	scales = D3DXVECTOR3(1.0f, 2.0f, 3.0f);

	// �� ������ �ؽ����� ���� �ٸ� �� ���� x, y �ְ� ���ڸ� �����մϴ�.
	distortion1 = D3DXVECTOR2(0.1f, 0.2f);
	distortion2 = D3DXVECTOR2(0.1f, 0.3f);
	distortion3 = D3DXVECTOR2(0.1f, 0.1f);

	// �ؽ��� ���ø� ��ǥ�� ������ ���� ũ�� �� ���̾ ���Դϴ�.
	distortionScale = 0.8f;
	distortionBias = 0.5f;

}

bool GraphicsClass::RenderEndScene()
{
	bool result;
	D3DXMATRIX MainSceneworldMatrix, orthoMatrix;
	Camera::Get()->GetOrtho(&orthoMatrix);
	D3DXMatrixIdentity(&MainSceneworldMatrix);

	D3D::Get()->BeginScene();

	result = m_Loading->Render(0, 0);
	if (!result) { return false; }

	result = m_Shader->RenderTextureShader(m_Loading->GetIndexCount(), MainSceneworldMatrix, ImageViewMatrix, orthoMatrix, m_Loading->GetTexture());
	if (!result) { return false; }


	D3D::Get()->EndScene();

	return true;
}

bool GraphicsClass::End()
{
	if (OffFire == true && OffTFire == true && OffTFire2 == true && m_UIManager->m_UI[0]->active==true)
	{
		PreShutdown();
		m_end = true;
	}

	return m_end;
}