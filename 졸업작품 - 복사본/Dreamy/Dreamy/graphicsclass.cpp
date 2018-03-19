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
	m_Model_Cube2 = 0;
	m_Model_Cube3 = 0;
	m_Model_CircleList = 0;

	m_Model_Mirror = 0;

	m_2D_Love = 0;
	m_Loading = 0;
	m_CrossHair = 0;

	m_Title = 0;

	m_Frustum = 0;

	m_RTTTexture = 0;

	m_Terrain = 0;
	m_TerrainShader = 0;
	//m_QuadTree = 0;

	m_Sky = 0;


	F1pressed = false;

	CharacterPos = { 512.5f, 30.0f, 310.0f };
	CharacterRot = { 0.0f, 0.0f, 0.0f };

	m_fbx = 0;

	//ȣ��
	m_Water = 0;
	//m_ReflectionTexture = 0;
	m_RefractionTexture = 0;
	m_WaterTerrain = 0;
	m_WaterTerrainShader = 0;

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

	result = m_2D_Love->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, L"../Dreamy/Main.jpg", 1600, 900);
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

	result = m_Loading->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, L"../Dreamy/Loading.jpg", 1600, 900);
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
	
	
	m_Model_Cube2 = new ModelClass;
	
	result = m_Model_Cube2->Initialize(m_D3D->GetDevice(), "../Dreamy/cube.txt", L"../Dreamy/seafloor.dds");
	if (!result) { MessageBox(hwnd, L"Could not initialize model object", L"Error", MB_OK); return false; }
	
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

	result = m_Terrain->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), "../Dreamy/setup.txt", "../Dreamy/dirt01d.tga", "../Dreamy/dirt01n.tga" );
	if (!result) { MessageBox(hwnd, L"Could not initialize Terrain object", L"Error", MB_OK); return false; }


	m_TerrainShader = new TerrainShaderClass;
	if (!m_TerrainShader) { return false; }

	m_TerrainShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) { MessageBox(hwnd, L"Could not initialize TerrainShader object", L"Error", MB_OK); return false; }
	
	//m_QuadTree = new QuadtreeClass;
	//if (!m_QuadTree) { return false; }
	//
	//result = m_QuadTree->Initialize(m_Terrain, m_D3D->GetDevice());
	//if (!result) { MessageBox(hwnd, L"Could not initialize QuadTree object", L"Error", MB_OK); return false; }

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
	if (!m_RefractionTexture)
	{
		return false;
	}

	// Initialize the refraction render to texture object.
	result = m_RefractionTexture->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the refraction render to texture object.", L"Error", MB_OK);
		return false;
	}

	//// Create the reflection render to texture object.
	//m_ReflectionTexture = new RTTTextureClass;
	//if (!m_ReflectionTexture)
	//{
	//	return false;
	//}
	//
	//// Initialize the reflection render to texture object.
	//result = m_ReflectionTexture->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
	//if (!result)
	//{
	//	MessageBox(hwnd, L"Could not initialize the reflection render to texture object.", L"Error", MB_OK);
	//	return false;
	//}
	// Create the water object.
	m_Water = new WaterClass;
	if (!m_Water)
	{
		return false;
	}

	// Initialize the water object.
	result = m_Water->Initialize(m_D3D->GetDevice(), L"../Dreamy/Data/waternormal.dds", 33.0f, 100.0f);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the water object.", L"Error", MB_OK);
		return false;
	}
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


	// FBX�� ����
	//-------------------------------------------------------------------------------------
	m_fbx = new FBXModel;
	if (!m_fbx) { return false; }
	
	result = m_fbx->Initialize(m_D3D->GetDevice());
	if(!result) { MessageBox(hwnd, L"fbx", L"Error", MB_OK); return false; }

	//-------------------------------------------------------------------------------------

	// 2D�̹��� ����
	//-------------------------------------------------------------------------------------
	m_CrossHair = new ImageClass;
	if (!m_CrossHair) { return false; }

	result = m_CrossHair->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, L"../Dreamy/crosshair.png", 1600, 900);
	if(!result) { MessageBox(hwnd, L"crosshair error", L"Error", MB_OK); return false; }
	//-------------------------------------------------------------------------------------


	return true;
}

// ��� �׷��� ��ü�� ���ᰡ ���⿡�� �Ͼ��.
void GraphicsClass::Shutdown()
{
	if (m_Title)	{ m_Title->Shutdown();	delete m_Title;	m_Title = 0;}
	if (m_2D_Love)	{ m_2D_Love->Shutdown(); delete m_2D_Love; m_2D_Love = 0;}
	if (m_CrossHair) { m_CrossHair->Shutdown(); delete m_CrossHair; m_CrossHair = 0; }
	//if (m_QuadTree) { m_QuadTree->Shutdown(); delete m_QuadTree; m_QuadTree = 0; }
	// Release the texture manager object.
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
	if (m_Terrain) { m_Terrain->Shutdown(); delete m_Terrain; m_Terrain = 0; }
	if (m_WaterTerrain) { m_WaterTerrain->Shutdownforwater(); delete m_WaterTerrain; m_WaterTerrain = 0; }
	if (m_Sky) { m_Sky->Shutdown(); delete m_Sky; m_Sky = 0; }
	if (m_Model_CircleList) { m_Model_CircleList->Shutdown(); delete m_Model_CircleList; m_Model_CircleList = 0; }
	if (m_Model_Cube) { m_Model_Cube->Shutdown(); delete m_Model_Cube; m_Model_Cube = 0; }
	if (m_Model_Cube2) { m_Model_Cube2->Shutdown(); delete m_Model_Cube2; m_Model_Cube2 = 0; }
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
bool GraphicsClass::Frame(int fps, int cpu, float frameTime, D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	bool result, foundHeight;
	D3DXVECTOR3 CameraPos;
	float height, Characterheight;

	// ���콺 ��������ġ ���� �� �ְ�
	//result = m_Title->SetMousePosition(mouseX, mouseY, m_D3D->GetDeviceContext());

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
	CharacterPos.x = pos.x;
	CharacterPos.z = pos.z+5.5f;


	//ĳ���� ȸ�� �ʱ�ȭ
	CharacterRot = D3DXVECTOR3(rot.x, rot.y, rot.z);

	//���� ��� ĳ���� �̵��� ����
	foundHeight = m_Terrain->GetHeightAtPosition(CharacterPos.x, CharacterPos.z, Characterheight);
	if (foundHeight)
	{
		CharacterPos.y = Characterheight + 20.0f;

	}

	//ī�޶� ��ġ �ʱ�ȭ
	m_Camera->SetPosition(D3DXVECTOR3(CharacterPos.x, CharacterPos.y, CharacterPos.z - 0.1f));
	//m_Camera->SetPosition(D3DXVECTOR3(512.5f, 30.0f, 300.0f));
	//m_Camera->SetRotation(D3DXVECTOR3(16.0f+rot.x, 0, 0));
	m_Camera->SetRotation(rot);

	//�ܹ��� �̵��ϴ� �ӵ�
	m_Water->Frame();

	//������ ������ ó���� �����Ѵ�.
	m_Sky->Frame(frameTime*0.00001f, 0.0f, frameTime*0.00002f, 0.0f);

	



	return true;
}


/*----------------------------------------------------------------------------------------
�̸� : Render()
�뵵 : ���׾��� RTT���� �׸���.
- �ݻ縦 �� ���� �켱 RTT�� �����ϰ� ���׾��� �ε��ؾ� ȥ�յȴ�.
------------------------------------------------------------------------------------------*/
bool GraphicsClass::Render( bool Pressed)
{
	bool result;

	//RTT����
	//-------------------------------------------------------------------------------------
	//result = RenderToTexture(); // �ſ�
	//if (!result) { return false; }
	
	//ȣ���� ���� �ؽ�ó
	result = RenderRefractionToTexture();
	if (!result) { return false; }

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
	float pitch = CharacterRot.x * 0.0174532925f;
	float yaw = CharacterRot.y * 0.0174532925f;
	float roll = CharacterRot.z * 0.0174532925f;
	D3DXVECTOR3 cameraPosition;


	D3DXMATRIX TerrainworldMatrix, SkyworldMatrix, WaterworldMatrix, PlaneworldMatrix, Plane2worldMatrix, CircleworldMatrix, Cube1worldMatrix, Cube2worldMatrix, Cube3worldMatrix, MirrorworldMatrix;
	D3DXMATRIX CrossHairworldMatrix;
	D3DXMATRIX FBXworldMatrix, FBXRotationMatrix;
	D3DXMATRIX TranslationMatrix, TranslationMatrix2, Cube3RotationMatrix;
	D3DXMATRIX viewMatrix, projectionMatrix, orthoMatrix, reflectionMatrix,WaterreflectionViewMatrix;
	D3DXMATRIX ScaleMatrix;

	D3DXMATRIX TextworldMatrix;

	int CircleCount, Circleindex;
	float positionX, positionY, positionZ, radius;
	D3DXVECTOR4 color; // CircleDiffuseColor
	bool Circlerender, result;

	// �ؽ�ó �̵�
	static float textureTranslation = 0.0f;
	textureTranslation += 0.003f;
	if (textureTranslation > 1.0f) { textureTranslation -= 1.0f; }

	//RTT����
	//-------------------------------------------------------------------------------------
	//result = RenderToTexture(); //���ϴ� ���� �ؽ�ó�� �׸���.
	//if (!result) { return false; }

	//�Ȱ� ��ü �ʱ�ȭ
	//fogEnd�� �־������� �����
	//-------------------------------------------------------------------------------------
	fogColor = 0.0f;
	fogStart = 150.0f;
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
	m_D3D->GetWorldMatrix(Plane2worldMatrix);
	m_D3D->GetWorldMatrix(CircleworldMatrix);
	m_D3D->GetWorldMatrix(Cube1worldMatrix);
	m_D3D->GetWorldMatrix(Cube2worldMatrix);
	m_D3D->GetWorldMatrix(Cube3worldMatrix);
	m_D3D->GetWorldMatrix(FBXworldMatrix);
	m_D3D->GetWorldMatrix(FBXRotationMatrix);
	m_D3D->GetWorldMatrix(MirrorworldMatrix);
	m_D3D->GetWorldMatrix(CrossHairworldMatrix);

	m_D3D->GetWorldMatrix(TranslationMatrix);
	m_D3D->GetWorldMatrix(TranslationMatrix2);

	m_D3D->GetWorldMatrix(TextworldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	m_D3D->GetOrthoMatrix(orthoMatrix);


	//�⺻ ��� ��ȯ
	//-------------------------------------------------------------------------------------
	// �����ϸ�->ȸ��->�̵� ������ ���ľ� �Ѵ�.
	D3DXMatrixTranslation(&SkyworldMatrix, cameraPosition.x, cameraPosition.y, cameraPosition.z);


	// FBX��
	D3DXMatrixTranslation(&FBXworldMatrix, CharacterPos.x+2.0f, CharacterPos.y+1.0f, CharacterPos.z);
	D3DXMatrixRotationX(&FBXRotationMatrix, 180.0f);
	D3DXMatrixMultiply(&FBXworldMatrix, &FBXRotationMatrix, &FBXworldMatrix);
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

			//result = m_Shader->RenderFogShader(m_D3D->GetDeviceContext(), m_Terrain->GetCellIndexCount(i), TerrainworldMatrix, viewMatrix,
			//	projectionMatrix, m_Terrain->GetColorTexture(), fogStart, fogEnd);
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
	D3DXMatrixTranslation(&WaterworldMatrix, 440.0f, m_Water->GetWaterHeight(), 750.0f);
	D3DXMatrixRotationY(&WaterRotationMatrix, 70.0f);
	D3DXMatrixMultiply(&WaterworldMatrix, &WaterRotationMatrix, &WaterworldMatrix);
	m_Water->Render(m_D3D->GetDeviceContext());

	result = m_Shader->RenderWaterShader(m_D3D->GetDeviceContext(), m_Water->GetIndexCount(), WaterworldMatrix, viewMatrix, projectionMatrix, WaterreflectionViewMatrix,
		m_RefractionTexture->GetShaderResourceView(), m_Sky->GetCloudTexture2(), m_Water->GetTexture(),
		m_Camera->GetPosition(), m_Water->GetNormalMapTiling(), m_Water->GetWaterTranslation(), m_Water->GetReflectRefractScale(),
		m_Water->GetRefractionTint(), m_Light->GetDirection(), m_Water->GetSpecularShininess());
	if (!result) { return false; }
	//-------------------------------------------------------------------------------------

	//�� ����Ʈ ������ �ʱ�ȭ
	//-------------------------------------------------------------------------------------
	CircleCount = m_Model_CircleList->GetModelCount();
	m_D3D->GetWorldMatrix(ScaleMatrix);
	D3DXMatrixScaling(&ScaleMatrix, 1.5f, 1.5f, 1.5f);
	//-------------------------------------------------------------------------------------

	// Scene ��� ��
	//-------------------------------------------------------------------------------------
	//�������� �ø� �� �� �� ����Ʈ
	for (Circleindex = 0; Circleindex < CircleCount; Circleindex++)
	{
		//����Ʈ�� �ִ°͵� ���ʴ�� �����͸� �о�´�.
		m_Model_CircleList->GetData(Circleindex, positionX, positionY, positionZ, color);

		radius = 1.0f;

		//Frustum Culling(�������� �ø�)
		//3D�𵨿� ��躼���� ������ ����ü�� ���̴��� üũ�Ѵ�.
		Circlerender = m_Frustum->CheckSphere(positionX + 580.0f, positionY + 20.0f, positionZ + 295.0f, radius);

		if (Circlerender)
		{
			//����Ʈ���� ���ǵȴ�� ��ġ ��ȯ
			D3DXMatrixTranslation(&CircleworldMatrix, positionX + 580.0f, positionY + 20.0f, positionZ + 295.0f);
			D3DXMatrixMultiply(&CircleworldMatrix, &ScaleMatrix, &CircleworldMatrix);

			//3D��(��ü) ������
			m_Model_Circle->Render(m_D3D->GetDeviceContext());

			//���̴� ������
			result = m_Shader->RenderLightShader(m_D3D->GetDeviceContext(), m_Model_Circle->GetIndexCount(), CircleworldMatrix, viewMatrix, projectionMatrix
				, m_Model_Circle->GetTexture(),m_Light->GetDirection(), m_Light->GetAmbientColor(), color
				, m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
			if (!result) { return false; }

			//���� ���� �ٸ� ��ġ�� ������ �ϴϱ� reset��Ų��!
			m_D3D->GetWorldMatrix(CircleworldMatrix);
			
			
		}

	}


	//ť��3 �ؽ�ó�̵��ϴ¾�
	D3DXMatrixTranslation(&Cube3worldMatrix, 600.0f, 15.0f, 338.0f);
	D3DXMatrixRotationY(&Cube3RotationMatrix, -40.0f);
	D3DXMatrixScaling(&ScaleMatrix, 7.0f, 7.0f, 7.0f);

	D3DXMatrixMultiply(&Cube3worldMatrix, &Cube3RotationMatrix, &Cube3worldMatrix);
	D3DXMatrixMultiply(&Cube3worldMatrix, &ScaleMatrix, &Cube3worldMatrix);

	//ť��3 �ؽ�ó �̵�+spec��	
	m_Model_Cube3->Render(m_D3D->GetDeviceContext());
	
	result = m_Shader->RenderTranslateShader(m_D3D->GetDeviceContext(), m_Model_Cube3->GetIndexCount(), Cube3worldMatrix, viewMatrix, projectionMatrix
		, m_Model_Cube3->GetTripleTextureArray(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
		m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower(), textureTranslation);
	if (!result) { return false; }	

	//ť��2 �Ȱ�
	D3DXMatrixTranslation(&Cube2worldMatrix, 613.0f, 15.0f, 350.0f);
	D3DXMatrixMultiply(&Cube2worldMatrix, &Cube3RotationMatrix, &Cube2worldMatrix);
	D3DXMatrixMultiply(&Cube2worldMatrix, &ScaleMatrix, &Cube2worldMatrix);

	//ť��2 �Ȱ� 
	m_Model_Cube2->Render(m_D3D->GetDeviceContext());

	result = m_Shader->RenderFogShader(m_D3D->GetDeviceContext(), m_Model_Cube2->GetIndexCount(), Cube2worldMatrix, viewMatrix, projectionMatrix,
		m_Model_Cube2->GetTexture(), fogStart, fogEnd);
	if (!result) { return false; }

	//ť��1 �븻��(������)
	D3DXMatrixTranslation(&Cube1worldMatrix, 626.0f, 15.0f, 363.0f);
	D3DXMatrixMultiply(&Cube1worldMatrix, &ScaleMatrix, &Cube1worldMatrix);
	D3DXMatrixMultiply(&Cube1worldMatrix, &Cube3RotationMatrix, &Cube1worldMatrix);

	//ť��1 �븻��(������)
	m_Model_Cube->Render(m_D3D->GetDeviceContext());

	result = m_Shader->RenderAlphaMapShader(m_D3D->GetDeviceContext(), m_Model_Cube->GetIndexCount(), Cube1worldMatrix, viewMatrix, projectionMatrix
		, m_Model_Cube->GetTripleTextureArray());
	if (!result) { return false; }
	//result = m_Shader->RenderTextureShader(m_D3D->GetDeviceContext(), m_Model_Cube->GetIndexCount(), Cube1worldMatrix, viewMatrix, projectionMatrix
	//	, m_Model_Cube->GetTexture());

	//���2 ����
	m_D3D->GetWorldMatrix(TranslationMatrix);
	D3DXMatrixRotationX(&Plane2worldMatrix, D3DXToRadian(90));
	//D3DXMatrixMultiply(&Plane2worldMatrix, &Plane2worldMatrix, &Cube3RotationMatrix);
	D3DXMATRIX Plane2RotationMatrix;
	D3DXMatrixRotationY(&Plane2RotationMatrix, D3DXToRadian(-45));
	D3DXMatrixMultiply(&Plane2worldMatrix, &Plane2worldMatrix, &Plane2RotationMatrix);
	D3DXMatrixTranslation(&TranslationMatrix, 633.0f, 30.0f, 334.0f);
	D3DXMatrixMultiply(&TranslationMatrix, &ScaleMatrix, &TranslationMatrix);
	D3DXMatrixMultiply(&TranslationMatrix, &Plane2worldMatrix, &TranslationMatrix);
	
	
	//���2 ����Ȱ�
	if (sibal == true)
	{
		m_D3D->TurnOnAlphaBlending();

		m_Model_Plane2->Render(m_D3D->GetDeviceContext());

		result = m_Shader->RenderTransparentShader(m_D3D->GetDeviceContext(), m_Model_Plane2->GetIndexCount(), TranslationMatrix, viewMatrix, projectionMatrix
			, m_Model_Plane2->GetTexture(), 0.7f);
		if (!result) { return false; }

		m_D3D->TurnOffAlphaBlending();
	}



	//fbx��
	//m_D3D->TurnOffCulling();
	//m_fbx->Render(m_D3D->GetDeviceContext());
	//result = m_Shader->RenderColorShader(m_D3D->GetDeviceContext(), m_fbx->GetIndexCount(), FBXworldMatrix, viewMatrix, projectionMatrix);
	//m_D3D->TurnOnCulling();

	//-------------------------------------------------------------------------------------

	/*
	//�⺻ ��� ��ȯ
	//-------------------------------------------------------------------------------------
	// �����ϸ�->ȸ��->�̵� ������ ���ľ� �Ѵ�.

	//ť��1
	D3DXMatrixRotationY(&Cube1worldMatrix, 0.6f);
	D3DXMatrixTranslation(&TranslationMatrix, -3, 4.5, -8);
	D3DXMatrixMultiply(&Cube1worldMatrix, &Cube1worldMatrix, &TranslationMatrix);


	//�ſ�
	D3DXMatrixTranslation(&MirrorworldMatrix, -2.7, 3.8, -8.9);
	D3DXMATRIX Scale;
	D3DXMatrixScaling(&Scale, 0.2f, 1.0f, 0.2f);
	D3DXMatrixMultiply(&MirrorworldMatrix, &Scale, &MirrorworldMatrix);


	//�⺻ ��� ��ȯ, 3D��, ���̴� ������, �������� �ø�
	//-------------------------------------------------------------------------------------

	//��� ���ĸ�
	m_Model_Plane->Render(m_D3D->GetDeviceContext());

	result = m_Shader->RenderAlphaMapShader(m_D3D->GetDeviceContext(), m_Model_Plane->GetIndexCount(), PlaneworldMatrix, viewMatrix, projectionMatrix
		, m_Model_Plane->GetTripleTextureArray());
	if (!result) { return false; }


	//�ſ�
	m_Model_Mirror->Render(m_D3D->GetDeviceContext());

	result = m_Shader->RenderReflectionShader(m_D3D->GetDeviceContext(), m_Model_Mirror->GetIndexCount(), MirrorworldMatrix, viewMatrix, projectionMatrix,
		m_Model_Mirror->GetTexture(), m_RTT->GetShaderResourceView(), reflectionMatrix);
	if (!result) { return false; }


	*/
	//-------------------------------------------------------------------------------------
	//2D�̹���, 2d���̴� ������, Text, Z���� ON/OFF, ���ĺ���, RTT�̴ϸ�
	//-------------------------------------------------------------------------------------
	m_D3D->TurnZBufferOff();

	m_D3D->TurnOnAlphaBlending();

	// ũ�ν���� �̹���.
	result = m_CrossHair->Render(m_D3D->GetDeviceContext(), 0, 0);
	if (!result) { return false; }

	result = m_Shader->RenderTextureShader(m_D3D->GetDeviceContext(), m_CrossHair->GetIndexCount(), CrossHairworldMatrix, baseViewMatrix, orthoMatrix, m_CrossHair->GetTexture());
	if (!result) { return false; }

	// Text���
	result = m_Title->SetPosition(cameraPosition.x, cameraPosition.y, cameraPosition.z, m_D3D->GetDeviceContext());
	result = m_Title->Render(m_D3D->GetDeviceContext(), TextworldMatrix, orthoMatrix);
	if (!result) { return false; }

	m_D3D->TurnOffAlphaBlending();
	m_D3D->TurnZBufferOn();
	//���ۿ� �׷��� ���� ȭ�鿡 ǥ���Ѵ�.
	m_D3D->EndScene();
	



	return true;
}

//-------------------------------------------------------------------------------------
//�̸�: RenderRefractionToTexture()
//�뵵: ȣ�� ������ ����� RTT�� ����(Refraction)�Ѵ�.
//-------------------------------------------------------------------------------------
bool GraphicsClass::RenderRefractionToTexture()
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

/*-----------------------------------------------------------------------------------------------
�̸�: TestInterSection()
�뵵:
- ������ �˻縦 ���� ���͸� �����ϰ� �ʿ��� ���� �˻�(��, ť��)������ ȣ���ϴ� �Ϲ� ������ �˻縦 �����ϴ� �Լ�.
- 2D���콺 ��ǥ�� �����ͼ� 3D���� ���ͷ� ��ȯ�Ѵ�.
- �� ���͸� Picking ray��� �θ���. �� picking ray�� ������ ������ ������, �� ���Ϳ� �浹�ϴ� 3d��ü�� ã�ƾ� �Ѵ�.
- ����->��->���������� �������� �ϸ� �ȴ�.
- 2D������ �����ͼ� ��������->��� 3D�������� ��ȯ�Ѵ�.

- �������� �ϴ� ���
1) ���콺 ��ǥ�� �����ͼ� ���� �࿡�� [-1,+1]������ �̵��Ͽ� �����Ѵ�.
2) �������� ����� ����� ȭ�� �������� ������.
3) �� ���� �̿��ؼ� �� �������� ���� ���͸� ��� ���� inverse view��Ʈ������ ���Ѵ�(�ݴ�ϱ�)
4) �� �������� ���� ������ ī�޶��� ��ġ�� ������ �� �ִ�.

5) �� ����+���� ���ͷ� ���� ���μ����� �Ϸ��� �� �ִ�.
6) ���������� ���� ����� ���� ���� ��ġ�� ��ȯ�Ѵ�. �װ��� �����ؼ� �����ش�.
7) ��������->��(����)->����(����)�� �������� �������� ������ ����ȭ �Ѵ�.
-------------------------------------------------------------------------------------------------*/
void GraphicsClass::TestIntersection(int mouseX, int mouseY, int m_screenWidth, int m_screenHeight)
{
	float pointX, pointY;
	D3DXMATRIX projectionMatrix;
	D3DXMATRIX viewMatrix, inverseViewMatrix;
	D3DXMATRIX worldMatrix, inverseWorldMatrix;
	D3DXMATRIX translateMatrix;

	D3DXVECTOR3 direction, origin;
	D3DXVECTOR3 rayOrigin, rayDirection;

	bool intersect, result;

	// ���콺 ��ǥ�� [-1,+1]������ �̵��Ѵ�.
	pointX = ((2.0f * (float)mouseX) / (float)m_screenWidth) - 1.0f;
	pointY = (((2.0f * (float)mouseY) / (float)m_screenHeight) - 1.0f) * -1.0f;

	// ���� ����� ����� ��ǥ���� ����Ʈ�� �������� ������???��???
	m_D3D->GetProjectionMatrix(projectionMatrix);
	pointX = pointX / projectionMatrix._11;
	pointY = pointY / projectionMatrix._22;
	
	//�� ��Ʈ������ inverse��Ų��.
	m_Camera->GetViewMatrix(viewMatrix);
	D3DXMatrixInverse(&inverseViewMatrix, NULL, &viewMatrix);

	// inverseViewMatrix�� �̿��ؼ� Picking ray�� ������ �����Ѵ�.
	direction.x = (pointX * inverseViewMatrix._11) + (pointY * inverseViewMatrix._21) + inverseViewMatrix._31;
	direction.y = (pointX * inverseViewMatrix._12) + (pointY * inverseViewMatrix._22) + inverseViewMatrix._32;
	direction.z = (pointX * inverseViewMatrix._13) + (pointY * inverseViewMatrix._23) + inverseViewMatrix._33;

	// ī�޶� ���������� picking ray�� ������ ���Ѵ�.
	origin = m_Camera->GetPosition();

	//���� ��ġ�� �ű�ٰ�? �̰�? ��..?????
	m_D3D->GetWorldMatrix(worldMatrix);
	//D3DXMatrixTranslation(&translateMatrix, 800.0f, 450.0f, 0.0f);
	D3DXMatrixTranslation(&translateMatrix, 600.0f, 15.0f, 340.0f);
	D3DXMatrixMultiply(&worldMatrix, &worldMatrix, &translateMatrix);

	// world����� �������ش�.
	D3DXMatrixInverse(&inverseWorldMatrix, NULL, &worldMatrix);

	// picking ray�� inverseWorld����� �����ش�.
	D3DXVec3TransformCoord(&rayOrigin, &origin, &inverseWorldMatrix);
	D3DXVec3TransformNormal(&rayDirection, &direction, &inverseWorldMatrix);

	// Normalize the ray direction.
	D3DXVec3Normalize(&rayDirection, &rayDirection);
	

	intersect = RaySphereIntersect(rayOrigin, rayDirection, 10.0f);

	if (intersect == true)
	{
		sibal = true;
	}
	else
		sibal = false;


	return;
}

bool GraphicsClass::RaySphereIntersect(D3DXVECTOR3 rayOrigin, D3DXVECTOR3 rayDirection, float radius)
{
	float a, b, c, discriminant;


	// Calculate the a, b, and c coefficients.
	a = (rayDirection.x * rayDirection.x) + (rayDirection.y * rayDirection.y) + (rayDirection.z * rayDirection.z);
	b = ((rayDirection.x * rayOrigin.x) + (rayDirection.y * rayOrigin.y) + (rayDirection.z * rayOrigin.z)) * 2.0f;
	c = ((rayOrigin.x * rayOrigin.x) + (rayOrigin.y * rayOrigin.y) + (rayOrigin.z * rayOrigin.z)) - (radius * radius);

	// Find the discriminant.
	discriminant = (b * b) - (4 * a * c);

	// if discriminant is negative the picking ray missed the sphere, otherwise it intersected the sphere.
	if (discriminant < 0.0f)
	{
		return false;
	}

	return true;
}