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
	//Camera::Get()->SetPosition(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	Camera::Get()->GetView(&ImageViewMatrix);
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
	attackPath = filePath + L"Horse_Walk.fbx";

	m_horse = new ModelScene();

	m_horse->LoadScene(tPosePath, true, true, true, false);
	m_horse->LoadScene(idlePath, false, false, false, true);
	m_horse->LoadScene(runPath, false, false, false, true);
	m_horse->LoadScene(attackPath, false, false, false, true);

	// Default Animation ����
	m_horse->SetCurrentAnimation(runPath);

	D3DXMATRIX world2;
	D3DXMatrixIdentity(&world2);
	D3DXMatrixTranslation(&world2, -40.0f, 10.0f, 0.0f);

	m_horse->SetWorldTransform(world2);
	//--------------------------------------------------------------------------------------

	// OBJ��
	//--------------------------------------------------------------------------------------
	m_cube = new ModelClass;
	if (!m_cube) { return false; }

	m_cube->Initialize("../Dreamy/Data/cube.txt", L"../Dreamy/Data/dirt01.dds");
	//--------------------------------------------------------------------------------------

	// Light
	//--------------------------------------------------------------------------------------
	m_Light = new LightClass;
	if (!m_Light) { return false; }

	m_Light->SetAmbientColor(0.05f, 0.05f, 0.05f, 1.0f);
	//m_Light->SetAmbientColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(-0.5f, -1.0f, -0.5f);
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

	result = m_Terrain->Initialize("../Dreamy/Data/setup.txt", "../Dreamy/Data/dirt01d.tga", "../Dreamy/Data/dirt01n.tga");
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

	// 2D�̹���
	//--------------------------------------------------------------------------------------
	m_MouseCursor = new ImageClass;
	if (!m_MouseCursor) { return false; }

	result = m_MouseCursor->Initialize(screenWidth, screenHeight, L"../Dreamy/Data/mouse.dds", 32, 32);
	if (!result) { MessageBox(hwnd, L"m_MouseCursor error", L"Error", MB_OK); return false; }
	//--------------------------------------------------------------------------------------


	return true;
}

// ��� �׷��� ��ü�� ���ᰡ ���⿡�� �Ͼ��.
void GraphicsClass::Shutdown()
{

	FBXShader::DeleteBuffer();
	SAFE_DELETE(m_horse);
	SAFE_DELETE(m_Light);
	SAFE_SHUTDOWN(m_TerrainShader);
	SAFE_SHUTDOWN(m_Terrain);
	SAFE_SHUTDOWN(m_Sky);
	SAFE_SHUTDOWN(m_Loading);
	SAFE_SHUTDOWN(m_CrossHair);
	SAFE_DELETE(m_Frustum);
	SAFE_SHUTDOWN(m_MouseCursor);
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
	// Model�� Frameó��
	//--------------------------------------------------------------------------------------
	m_horse->Update();

	m_Sky->Frame(frameTime*0.00001f, 0.0f, frameTime*0.00002f, 0.0f);
	//--------------------------------------------------------------------------------------
	
	// Camera Frameó��
	//--------------------------------------------------------------------------------------
	D3DXVECTOR3 CameraPos;
	bool foundHeight;
	float CameraY;
	Camera::Get()->GetPosition(&CameraPos);
	
	foundHeight = m_Terrain->GetHeightAtPosition(CameraPos.x, CameraPos.z, CameraY);
	if (foundHeight)
		CameraPos.y = CameraY + 30.0f;

	Camera::Get()->SetPosition(CameraPos);
	Camera::Get()->Update();
	//--------------------------------------------------------------------------------------

	// ���콺 
	//--------------------------------------------------------------------------------------
	MousePosX = mouseX;
	MousePosY = mouseY;
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
	fogStart = 250.0f;
	fogEnd = 550.0f;
	//-------------------------------------------------------------------------------------

	//�⺻���
	//-------------------------------------------------------------------------------------
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
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
	m_Shader->RenderSkydomeShader( m_Sky->GetIndexCount(), SkyworldMatrix, viewMatrix, projectionMatrix,
		m_Sky->GetApexColor(), m_Sky->GetCenterColor());

	//�ø�����ѱ�
	Rasterizer::Get()->SetOnCullMode();

	//���ĺ��� ȥ�� ���� ����
	Blender::Get()->SetAdd();

	m_Sky->RenderCloud();

	m_Shader->RenderCloudShader( m_Sky->GetCloudIndexCount(), SkyworldMatrix, viewMatrix, projectionMatrix,
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
		result = m_Terrain->RenderCell( i, m_Frustum);
		if (result)
		{

			result = m_TerrainShader->Render( m_Terrain->GetCellIndexCount(i), worldMatrix, viewMatrix,
				projectionMatrix, m_Terrain->GetColorTexture(), m_Terrain->GetNormalMapTexture(), m_Light->GetDirection(), m_Light->GetDiffuseColor(), fogStart, fogEnd);
			if (!result) { return false; }

			//m_Terrain->RenderCellLines(m_D3D->GetDeviceContext(), i);
			//m_Shader->RenderColorShader(m_D3D->GetDeviceContext(), m_Terrain->GetCellLinesIndexCount(i), TerrainworldMatrix, viewMatrix, projectionMatrix);
			//if (!result) { return false; }
		}
	}
	//-------------------------------------------------------------------------------------

	// Model��
	//-------------------------------------------------------------------------------------
	m_horse->Render();

	m_cube->Render();
	m_Shader->RenderTextureShader(m_cube->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_cube->GetTexture());
	//-------------------------------------------------------------------------------------

	// 2D�̹���
	//-------------------------------------------------------------------------------------
	m_MouseCursor->Render(MousePosX, MousePosY);
	
	//-------------------------------------------------------------------------------------



	//���ۿ� �׷��� ���� ȭ�鿡 ǥ���Ѵ�.
	D3D::Get()->EndScene();
	



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