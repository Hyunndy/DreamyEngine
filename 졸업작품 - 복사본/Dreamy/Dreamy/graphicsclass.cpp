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

	// Default Animation ����
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

	// Default Animation ����
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

	// OBJ��
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

	result = m_Sky->InitializeCloud( L"../Dreamy/Data/cloud001.dds", L"../Dreamy/Data/cloud002.dds");
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

	m_UI = new ImageClass;
	
	result = m_UI->Initialize(screenWidth, screenHeight, L"../Dreamy/Data/UI.png", screenWidth, screenHeight);
	if (!result) { MessageBox(hwnd, L"UI error", L"Error", MB_OK); return false; }

	//--------------------------------------------------------------------------------------

	// �ν��Ͻ� ��ü�� ����
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

	//744 51 554 x��Ŀ����
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

	// �̴ϸ�
	//--------------------------------------------------------------------------------------
	m_Minimap = new MinimapClass;
	if (!m_Minimap) { return false; }

	result = m_Minimap->Initialize( screenWidth, screenHeight, 1025, 1025);
	if (!result) { MessageBox(hwnd, L"minimap error", L"Error", MB_OK); return false; }
	//--------------------------------------------------------------------------------------

	// ������ ��ü(��ƼŬ, ���� ��)
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

// ��� �׷��� ��ü�� ���ᰡ ���⿡�� �Ͼ��.
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
bool GraphicsClass::Frame(int fps, int cpu, float frameTime, D3DXVECTOR3 pos, D3DXVECTOR3 rot, int mouseX, int mouseY)
{
	bool result;


	// Model�� Frameó��
	//--------------------------------------------------------------------------------------
	//m_horse->Update(); //��1
	m_npc->Update(); //��2

	m_Sky->Frame(frameTime*0.00001f, 0.0f, frameTime*0.00002f, 0.0f); //����
	m_Particle->Frame(frameTime); // ��ƼŬ
	m_Minimap->PositionUpdate(CameraPos.x, CameraPos.z); //�̴ϸ�
	m_Water->Frame(frameTime);
	//--------------------------------------------------------------------------------------
	
	// Camera Frameó��
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

	// ���콺 
	//--------------------------------------------------------------------------------------
	MousePosX = mouseX;
	MousePosY = mouseY;
	//--------------------------------------------------------------------------------------

	// �ؽ�Ʈ Frameó��
	//--------------------------------------------------------------------------------------
	// fps����
	result = m_Text->SetFps(fps);
	if (!result) { return false; }

	// cpu����
	result = m_Text->SetCpu(cpu);
	if (!result) { return false; }
	//--------------------------------------------------------------------------------------
	return true;
}


/*----------------------------------------------------------------------------------------
�̸� : Render()
�뵵 : ���׾��� RTT���� �׸���.
- �ݻ縦 �� ���� �켱 RTT�� �����ϰ� ���׾��� �ε��ؾ� ȥ�յȴ�.
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


	//�Ȱ� ��ü �ʱ�ȭ
	//fogEnd�� �־������� �����
	//-------------------------------------------------------------------------------------
	float fogColor, fogStart, fogEnd;
	fogColor = 0.0f;
	fogStart = 400.0f;
	fogEnd = 550.0f;
	//-------------------------------------------------------------------------------------

	//�⺻���
	//-------------------------------------------------------------------------------------
	D3DXMatrixIdentity(&worldMatrix);
	Camera::Get()->GetView(&viewMatrix);
	Camera::Get()->GetProjection(&projectionMatrix);
	Camera::Get()->GetOrtho(&orthoMatrix);

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
	D3DXVECTOR3 cameraPosition;
	Camera::Get()->GetPosition(&cameraPosition);
	D3DXMatrixTranslation(&SkyworldMatrix, cameraPosition.x, cameraPosition.y, cameraPosition.z);

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
		m_Sky->GetCloudTexture1(), m_Sky->GetCloudTexture2(), m_Sky->GetTranslation(0), m_Sky->GetTranslation(1),
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

			//m_Terrain->RenderCellLines(m_D3D->GetDeviceContext(), i);
			//m_Shader->RenderColorShader(m_D3D->GetDeviceContext(), m_Terrain->GetCellLinesIndexCount(i), TerrainworldMatrix, viewMatrix, projectionMatrix);
			//if (!result) { return false; }
		}
	}
	//-------------------------------------------------------------------------------------

	// ȣ��  512.5f, 30.0f, 310.0f
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

	// FBXANIMATIONModel��
	//-------------------------------------------------------------------------------------
	//m_horse->Render();
	m_npc->Render();

	// OBJ��
	//-------------------------------------------------------------------------------------

	// ����ŧ���� �ڽ�
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

	// ��
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

	// ��ƼŬ
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

	// �ν��Ͻ�, ��������ü
	//-------------------------------------------------------------------------------------

	// �ν��Ͻ�
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




	// ���� ������
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



	//���ۿ� �׷��� ���� ȭ�鿡 ǥ���Ѵ�.
	D3D::Get()->EndScene();
	



	return true;
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