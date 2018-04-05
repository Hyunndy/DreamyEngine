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


	//ȣ��
	m_Water = 0;
	//m_ReflectionTexture = 0;
	m_RefractionTexture = 0;
	m_WaterTerrain = 0;
	m_WaterTerrainShader = 0;

	//��
	m_Fire_Effect = 0;
	frameTime = 0.0f;
	m_Particle = 0;
	m_Particleactive = false;

	//������
	m_Billboard_Tree = 0;

	//�̴ϸ�
	m_Minimap = 0;

	//�ν��Ͻ�
	m_Instancing = 0;

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
	m_D3D = new D3DClass;

	if (!m_D3D) { return false; }

	/* Direct3D ��ü�� �ʱ�ȭ�մϴ�.
	 �� �Լ����� ȭ���� �ʺ�, ����, ������ �ڵ�, Graphicsclass.h�� ���ǵ� �� ���� ���� �������� �����Ѵ�.
	 D3DCLASS���� �� �������� ����Ͽ� Direct3D �ý����� �����Ѵ�. */
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result) { MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK); return false; }

	//--------------------------------------------------------------------------------------

	// ī�޶�ü ����
	//-------------------------------------------------------------------------------------
	m_Camera = new CameraClass;

	if (!m_Camera) { return false; }

	//�۲��� ��ġ
	m_Camera->SetPosition(D3DXVECTOR3(0.0f, 0.0f, -1.0f));
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);
	//-------------------------------------------------------------------------------------


	// ���̴� ��ü ����
	//-------------------------------------------------------------------------------------
	m_Shader = new ShaderManagerClass;
	if (!m_Shader) { return false; }

	result = m_Shader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) { MessageBox(hwnd, L"Could not initialize the color shader object", L"Error", MB_OK); return false; }
	//-------------------------------------------------------------------------------------


	// �����̹��� ��ü ����
	//-------------------------------------------------------------------------------------
	m_2D_Love = new ImageClass;

	result = m_2D_Love->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, L"../Dreamy/Main.jpg", screenWidth, screenHeight);
	if (!result) { MessageBox(hwnd, L"Could not initialize model object", L"Error", MB_OK); return false; }
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

	// �ε��̹��� ��ü ����
	//-------------------------------------------------------------------------------------
	m_Loading = new ImageClass;

	result = m_Loading->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, L"../Dreamy/Loading.jpg", screenWidth, screenHeight);
	if (!result) { MessageBox(hwnd, L"Could not initialize model object", L"Error", MB_OK); return false; }
	//-------------------------------------------------------------------------------------

	// �ε��̹��� ���
	RenderLoadingScene();


	// ī�޶� ��ü ����
	//-------------------------------------------------------------------------------------
	m_minimapCamera = new CameraClass;
	if (!m_minimapCamera) { return false; }

	//�̴ϸ� ī�޶� 
	m_minimapCamera->SetPosition(D3DXVECTOR3(0.0f, 0.0f, -2.5f));
	m_minimapCamera->SetRotation(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	//-------------------------------------------------------------------------------------



	// Text��ü ����
	//-------------------------------------------------------------------------------------
	m_Title = new TextClass;

	if (!m_Title) { return false; }

	result = m_Title->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix);
	if (!result) { MessageBox(hwnd, L"Could not initialize TitleText", L"Error", MB_OK); return false; }
	//-------------------------------------------------------------------------------------

	// ������ ��ü ����(����Ʈ,���� ��)
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

	// 3D�� ��ü ����
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

	// Frustum ��ü ����
	//-------------------------------------------------------------------------------------
	m_Frustum = new FrustumClass;
	if (!m_Frustum) { return false; }
	//-------------------------------------------------------------------------------------


	//Terrain, sky, ȣ�� ��ü ����
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

	// ȣ��
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

	// �� ��ü ����
	//-------------------------------------------------------------------------------------
	m_Light = new LightClass;
	if (!m_Light) { return false; }

	m_Light->SetAmbientColor(0.05f, 0.05f, 0.05f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(-0.5f, -1.0f, -0.5f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(16.0f); // �ݻ簭������ �������� �ݻ籤 ȿ���� Ŀ����!
	//-------------------------------------------------------------------------------------

    // ModelList��ü ����
    //-------------------------------------------------------------------------------------
	m_Model_CircleList = new ModelManagerClass;
	if (!m_Model_CircleList) { return false; }
	
	// �� 4���� ������!
	m_Model_CircleList->Initialize(5);
	if (!result) { MessageBox(hwnd, L"����Ʈ ����µ� ������", L"Error", MB_OK); return false; }

	//-------------------------------------------------------------------------------------


	// RTT ��ü ����
	//-------------------------------------------------------------------------------------
	//m_RTT = new RTTClass;
	//if (!m_RTT) { return false; }
	//
	//// ��� ȭ���� �ؽ�ó�� �׸��� �ͱ� ������ ȭ���� �ʺ�� ���̸� �ؽ����� ũ��� �����ߵ�.
	//
	//result = m_RTT->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight);
	//if (!result) { MessageBox(hwnd, L"����Ʈ ����µ� ������", L"Error", MB_OK); return false; }
	//-------------------------------------------------------------------------------------

	// RTT �ؽ�ó(�ſ�, �� ǥ��) ��ü ����
	//-------------------------------------------------------------------------------------

	// �ν��Ͻ� ��ü�� ����
	//-------------------------------------------------------------------------------------
	m_Instancing = new InstancingClass;
	if (!m_Instancing) { return false; }

	m_Instancing->SetInstanceCount(4);
	m_Instancing->SetInstancePosition(659.0f, 48.0f, 784.0f);
	
	result = m_Instancing->Initialize(m_D3D->GetDevice(), "../Dreamy/Data/square2.txt", L"../Dreamy/Data/redtree.png");
	if(!result) { MessageBox(hwnd, L"instancing", L"Error", MB_OK); return false; }

	//-------------------------------------------------------------------------------------

	// FBX�� ����
	//-------------------------------------------------------------------------------------
	m_fbx = new FBXModel;
	if (!m_fbx) { return false; }
	
	result = m_fbx->Initialize(m_D3D->GetDevice(), "../Dreamy/Data/Horse.fbx", L"../Dreamy/Data/Horse_D.png");
	if(!result) { MessageBox(hwnd, L"fbx", L"Error", MB_OK); return false; }



	//-------------------------------------------------------------------------------------

	// 2D�̹��� ����
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

	// �̴ϸ�
	//-------------------------------------------------------------------------------------
	m_Minimap = new MinimapClass;
	if (!m_Minimap) { return false; }

	result = m_Minimap->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, 1025, 1025);
	if (!result) { MessageBox(hwnd, L"minimap error", L"Error", MB_OK); return false; }
	//-------------------------------------------------------------------------------------

	return true;
}

// ��� �׷��� ��ü�� ���ᰡ ���⿡�� �Ͼ��.
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
	bool result, foundHeight;
	D3DXVECTOR3 CameraPos;
	float height, Characterheight;

	MousePosX = mouseX;
	MousePosY = mouseY;

	// fps����
	result = m_Title->SetFps(fps, m_D3D->GetDeviceContext());
	if (!result) { return false; }


	// cpu����
	result = m_Title->SetCpu(cpu, m_D3D->GetDeviceContext());
	if (!result) { return false; }


	//m_Camera->SetRotation(rot);


	 //���� ��� ī�޶� �̵��� ����
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

	//ĳ���� ��ġ �ʱ�ȭ
	//CharacterPos.x = pos.x;
	//CharacterPos.z = pos.z+5.5f;
	//
	////ĳ���� ȸ�� �ʱ�ȭ
	//CharacterRot = D3DXVECTOR3(rot.x, rot.y, rot.z);

	CameraPos = pos;
	
	//���� ��� ĳ���� �̵��� ����
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
	//ī�޶� ��ġ �ʱ�ȭ
	//m_Camera->SetPosition(D3DXVECTOR3(CharacterPos.x, CharacterPos.y, CharacterPos.z - 0.1f));
	m_Camera->SetPosition(CameraPos);
	//m_Camera->SetPosition(D3DXVECTOR3(512.5f, 30.0f, 300.0f));
	//m_Camera->SetRotation(D3DXVECTOR3(16.0f+rot.x, 0, 0));
	m_Camera->SetRotation(rot);

	//�ܹ��� �̵��ϴ� �ӵ�
	m_Water->Frame(frameTime);

	//������ ������ ó���� �����Ѵ�.
	m_Sky->Frame(frameTime*0.00001f, 0.0f, frameTime*0.00002f, 0.0f);

	//�̴ϸ�
	m_Minimap->PositionUpdate(CameraPos.x, CameraPos.z);

	//��ƼŬ
	m_Particle->Frame(frameTime, m_D3D->GetDeviceContext());
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

	//RTT����
	//-------------------------------------------------------------------------------------
	//result = RenderToTexture(); // �ſ�
	//if (!result) { return false; }
	//if (frametime%15==0)
	//{
	////ȣ���� ���� �ؽ�ó
	result = RenderRefractionToTexture(Pressed);
	if (!result) { return false; }
	//}
	//ȣ���� �ݻ� �ؽ�ó
	//result = RenderReflectionToTexture();
	//if (!result) { return false; }
	//-------------------------------------------------------------------------------------

	// ���ξ� �׸�
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

	// �ؽ�ó �̵�
	//-------------------------------------------------------------------------------------
	static float textureTranslation = 0.0f;
	textureTranslation += 0.003f;
	if (textureTranslation > 1.0f) { textureTranslation -= 1.0f; }
	//-------------------------------------------------------------------------------------

	// ����Ʈ ���� ����
	//-------------------------------------------------------------------------------------
	SetEffectVariable();
	//-------------------------------------------------------------------------------------

	//�Ȱ� ��ü �ʱ�ȭ
	//fogEnd�� �־������� �����
	//-------------------------------------------------------------------------------------
	fogColor = 0.0f;
	fogStart = 250.0f;
	fogEnd = 550.0f;
	//-------------------------------------------------------------------------------------

	//����
	//-------------------------------------------------------------------------------------
	m_D3D->BeginScene(fogColor, fogColor, fogColor, 1.0f);

	//ī�޶� ��ġ
	//-------------------------------------------------------------------------------------
	m_Camera->Render();
	cameraPosition = m_Camera->GetPosition();

	//m_Camera->RenderWaterReflection(m_Water->GetWaterHeight());

	//�⺻ ����,��,����,���翵 ��� ȹ��
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

	//�⺻ ��� ��ȯ
	//-------------------------------------------------------------------------------------
	// �����ϸ�->ȸ��->�̵� ������ ���ľ� �Ѵ�.
	D3DXMatrixTranslation(&SkyworldMatrix, cameraPosition.x, cameraPosition.y, cameraPosition.z);
	


	//-------------------------------------------------------------------------------------

	//�þ� ����ü ����
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

	//����
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
	
	// Terrain���� ���� �� ������ŭ ���� �ε��Ѵ�.
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


	// ȣ��  512.5f, 30.0f, 310.0f
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
	
	//�� ����Ʈ ������ �ʱ�ȭ
	//-------------------------------------------------------------------------------------
	CircleCount = m_Model_CircleList->GetModelCount();
	//-------------------------------------------------------------------------------------
	
	// Scene ��� ��
	//-------------------------------------------------------------------------------------
	
	//�������� �ø� �� �� �� ����Ʈ
	D3DXMATRIX CircleWorldMatrix;
	m_D3D->GetWorldMatrix(CircleWorldMatrix);
	m_Model_Circle->Scale(1.5f, 1.5f, 1.5f);
	
	for (Circleindex = 0; Circleindex < CircleCount; Circleindex++)
	{
		//����Ʈ�� �ִ°͵� ���ʴ�� �����͸� �о�´�.
		m_Model_CircleList->GetData(Circleindex, positionX, positionY, positionZ, color);
	
		radius = 1.0f;
		//755.0f, 26.0f, 389.0f
		//Frustum Culling(�������� �ø�)
		//3D�𵨿� ��躼���� ������ ����ü�� ���̴��� üũ�Ѵ�.
		Circlerender = m_Frustum->CheckSphere(positionX + 730.0f, positionY + 26.0f, positionZ + 375.0f, radius);
	
		if (Circlerender)
		{
			//����Ʈ���� ���ǵȴ�� ��ġ ��ȯ
			m_Model_Circle->Translation(positionX + 730.0f, positionY + 26.0f, positionZ + 375.0f);
			m_Model_Circle->Multiply(m_Model_Circle->GetScailingMatrix(), m_Model_Circle->GetTranslationMatrix());
			D3DXMatrixMultiply(&CircleWorldMatrix, &m_Model_Circle->GetFinalMatrix(), &CircleWorldMatrix);
	
			//3D��(��ü) ������
			m_Model_Circle->Render(m_D3D->GetDeviceContext());
	
			//���̴� ������
			result = m_Shader->RenderLightShader(m_D3D->GetDeviceContext(), m_Model_Circle->GetIndexCount(), CircleWorldMatrix, viewMatrix, projectionMatrix
				, m_Model_Circle->GetTexture(),m_Light->GetDirection(), m_Light->GetAmbientColor(), color
				, m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
			if (!result) { return false; }
	
			//���� ���� �ٸ� ��ġ�� ������ �ϴϱ� reset��Ų��!
			m_D3D->GetWorldMatrix(CircleWorldMatrix);
			
		}
	}
	
	//ť��1 ���ĸ�
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
	
	
	//ť��3 ������+����ŧ����+�ؽ�ó�̵�
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
	
	
	//���2 ����
	D3DXMATRIX Plane2worldMatrix;
	m_D3D->GetWorldMatrix(Plane2worldMatrix);
	
	m_Model_Plane2->RotationX(D3DXToRadian(90));
	m_Model_Plane2->RotationY(D3DXToRadian(-45));
	m_Model_Plane2->RotationMultiply(m_Model_Plane2->GetRotationXMatrix(), m_Model_Plane2->GetRotationYMatrix());
	m_Model_Plane2->Translation(784.0f, 40.0f, 371.0f);
	
	m_Model_Plane2->Multiply(m_Model_Plane2->GetRotationMatrix(), m_Model_Plane2->GetTranslationMatrix());
	m_Model_Plane2->Multiply(m_Model_Cube3->GetScailingMatrix(), m_Model_Plane2->GetFinalMatrix());
	D3DXMatrixMultiply(&Plane2worldMatrix, &m_Model_Plane2->GetFinalMatrix(), &Plane2worldMatrix);
	
	
	//���2 ����
	if (sibal == true)
	{
		m_D3D->TurnOnAlphaBlending();
	
		m_Model_Plane2->Render(m_D3D->GetDeviceContext());
	
		result = m_Shader->RenderTransparentShader(m_D3D->GetDeviceContext(), m_Model_Plane2->GetIndexCount(), Plane2worldMatrix, viewMatrix, projectionMatrix
			, m_Model_Plane2->GetTexture(), 0.7f);
		if (!result) { return false; }
	
		m_D3D->TurnOffAlphaBlending();
	}
	
	//������, ����Ʈ
	//-------------------------------------------------------------------------------------
	
	//��ƼŬ
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

	//��
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
	//2D�̹���, 2d���̴� ������, Text, Z���� ON/OFF, ���ĺ���, RTT�̴ϸ�
	//-------------------------------------------------------------------------------------
	m_D3D->TurnZBufferOff();

	m_D3D->TurnOnAlphaBlending();
//	D3DXMATRIX TextScaleMatrix;
//	D3DXMatrixScaling(&TextScaleMatrix, 2.0f, 2.0f, 2.0f);
//	D3DXMatrixMultiply(&TextworldMatrix, &TextScaleMatrix, &TextworldMatrix);
	// Text���
	// ���콺 ��������ġ ���� �� �ְ�
	result = m_Title->SetMousePosition(MousePosX, MousePosY, m_D3D->GetDeviceContext());
	result = m_Title->SetPosition(cameraPosition.x, cameraPosition.y, cameraPosition.z, m_D3D->GetDeviceContext());
	result = m_Title->Render(m_D3D->GetDeviceContext(), TextworldMatrix, orthoMatrix);
	if (!result) { return false; }
	result = m_Minimap->Render(m_D3D->GetDeviceContext(), m_Shader->m_TextureShader, CrossHairworldMatrix, baseViewMatrix, orthoMatrix);
	if (!result) { return false; }



	// ũ�ν���� �̹���.
	result = m_CrossHair->Render(m_D3D->GetDeviceContext(), 0, 0);
	if (!result) { return false; }
	result = m_Shader->RenderTextureShader(m_D3D->GetDeviceContext(), m_CrossHair->GetIndexCount(), CrossHairworldMatrix, baseViewMatrix, orthoMatrix, m_CrossHair->GetTexture());
	if (!result) { return false; }


	// ���콺 Ŀ��
	result = m_Cursor->Render(m_D3D->GetDeviceContext(), MousePosX, MousePosY);  if (!result) { return false; }
	D3DXMATRIX MouseWorldMatrix;
	D3DXMatrixScaling(&MouseWorldMatrix, 1.5f, 1.5f, 1.5f);
	result = m_Shader->RenderTextureShader(m_D3D->GetDeviceContext(), m_Cursor->GetIndexCount(), MouseWorldMatrix, baseViewMatrix, orthoMatrix, m_Cursor->GetTexture());


	m_D3D->TurnOffAlphaBlending();
	m_D3D->TurnZBufferOn();

	// FBX��
	D3DXMatrixTranslation(&FBXworldMatrix, 946.0f, 40.0f, 464.0f);
	D3DXMatrixScaling(&FBXRotationMatrix, 0.1f, 0.1f, 0.1f);

	D3DXMatrixMultiply(&FBXworldMatrix, &FBXRotationMatrix, &FBXworldMatrix);

	//fbx��
	m_D3D->TurnOffCulling();
	m_fbx->Render(m_D3D->GetDeviceContext());
	result = m_Shader->RenderTextureShader(m_D3D->GetDeviceContext(), m_fbx->GetIndexCount(), FBXworldMatrix, viewMatrix, projectionMatrix, m_fbx->GetTexture());

	m_D3D->TurnOnCulling();


	//-------------------------------------------------------------------------------------

	/*
	//�⺻ ��� ��ȯ
	//-------------------------------------------------------------------------------------
	// �����ϸ�->ȸ��->�̵� ������ ���ľ� �Ѵ�.


	//�ſ�
	D3DXMatrixTranslation(&MirrorworldMatrix, -2.7, 3.8, -8.9);
	D3DXMATRIX Scale;
	D3DXMatrixScaling(&Scale, 0.2f, 1.0f, 0.2f);
	D3DXMatrixMultiply(&MirrorworldMatrix, &Scale, &MirrorworldMatrix);


	//�⺻ ��� ��ȯ, 3D��, ���̴� ������, �������� �ø�
	//-------------------------------------------------------------------------------------

	//�ſ�
	m_Model_Mirror->Render(m_D3D->GetDeviceContext());

	result = m_Shader->RenderReflectionShader(m_D3D->GetDeviceContext(), m_Model_Mirror->GetIndexCount(), MirrorworldMatrix, viewMatrix, projectionMatrix,
		m_Model_Mirror->GetTexture(), m_RTT->GetShaderResourceView(), reflectionMatrix);
	if (!result) { return false; }


	*/

	//���ۿ� �׷��� ���� ȭ�鿡 ǥ���Ѵ�.
	m_D3D->EndScene();
	



	return true;
}

//-------------------------------------------------------------------------------------
//�̸�: RenderRefractionToTexture()
//�뵵: ȣ�� ������ ����� RTT�� ����(Refraction)�Ѵ�.
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
	////����
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
�̸� : RenderToTexture()
�뵵 :
- RTT����� ����� �� �ְ��ϴ� �Լ�
- ��� �ݻ� ����� ����ϱ� ���� �ݻ� �� ����� �����ؾ� �Ѵ�.
------------------------------------------------------------------------------------------*/
bool GraphicsClass::RenderToTexture()
{
	//bool result;
	//D3DXMATRIX worldMatrix, reflectionViewMatrix, projectionMatrix;
	//D3DXMATRIX TranslationMatrix;
	//
	////RTT����� �ʱ�ȭ��Ų��. = ������ Ÿ���� rtt�� �����Ѵ�.
	//m_RTTTexture->SetRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView());
	//
	//// RTT�� �ʱ�ȭ �Ѵ�.
	//m_RTTTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);
	//
	////// �� �Լ��� ���� �׸��� ����� ��� RTT�� ������ �ȴ�.
	////result = RenderRTTScene();
	////if (!result)
	////{
	////	return false;
	////}
	//
	////ī�޶� �ݻ� ����� ����ϵ��� �Ѵ�. 
	//m_Camera->RenderReflection(3.5f, 4.5f);
	//
	//
	////��ĵ�
	//reflectionViewMatrix = m_Camera->GetReflectionViewMatrix();
	//m_D3D->GetWorldMatrix(worldMatrix);
	//m_D3D->GetProjectionMatrix(projectionMatrix);
	//D3DXMatrixRotationY(&worldMatrix, 0.6f);
	//D3DXMatrixTranslation(&TranslationMatrix, -3 ,4.5, -8);
	//D3DXMatrixMultiply(&worldMatrix, &worldMatrix, &TranslationMatrix);
	//
	////ť��1
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
	//// ������ Ÿ���� RTT���� �ٽ� ����۷� ������.
	//m_D3D->SetBackBufferRenderTarget();
	//
	return true;
}

/*----------------------------------------------------------------------------------------
�̸� : RenderScene()
�뵵 : RTT�� ����ϱ� ���� RTT�� �׷��� ��
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
�̸� : RenderLoadingScene()
�뵵 : �ε�ȭ���� �׸���.
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
�̸� : RenderMainScene()
�뵵 : ����ȭ���� �׸���.
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
	// �Ҳ� ����Ʈ
	//-------------------------------------------------------------------------------------
	frameTime += 0.01f;
	if (frameTime > 1000.0f) { frameTime = 0.0f; }

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

float GraphicsClass::CalculateBillboarding(D3DXVECTOR3 cameraPosition, D3DXVECTOR3 BillboardPosition)
{
	float angle;
	float rotation;
	angle = atan2(BillboardPosition.x - cameraPosition.x, BillboardPosition.z - cameraPosition.z) * (180.0 / D3DX_PI);
	rotation = (float)angle * 0.0174532925f;

	return rotation;
}