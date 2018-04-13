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
	//Camera::Get()->SetPosition(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	Camera::Get()->GetView(&ImageViewMatrix);
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
	attackPath = filePath + L"Horse_Walk.fbx";

	m_horse = new ModelScene();

	m_horse->LoadScene(tPosePath, true, true, true, false);
	m_horse->LoadScene(idlePath, false, false, false, true);
	m_horse->LoadScene(runPath, false, false, false, true);
	m_horse->LoadScene(attackPath, false, false, false, true);

	// Default Animation 설정
	m_horse->SetCurrentAnimation(runPath);

	D3DXMATRIX world2;
	D3DXMatrixIdentity(&world2);
	D3DXMatrixTranslation(&world2, -40.0f, 10.0f, 0.0f);

	m_horse->SetWorldTransform(world2);
	//--------------------------------------------------------------------------------------

	// OBJ모델
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

	// 2D이미지
	//--------------------------------------------------------------------------------------
	m_MouseCursor = new ImageClass;
	if (!m_MouseCursor) { return false; }

	result = m_MouseCursor->Initialize(screenWidth, screenHeight, L"../Dreamy/Data/mouse.dds", 32, 32);
	if (!result) { MessageBox(hwnd, L"m_MouseCursor error", L"Error", MB_OK); return false; }
	//--------------------------------------------------------------------------------------


	return true;
}

// 모든 그래픽 객체의 종료가 여기에서 일어난다.
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
	// Model들 Frame처리
	//--------------------------------------------------------------------------------------
	m_horse->Update();

	m_Sky->Frame(frameTime*0.00001f, 0.0f, frameTime*0.00002f, 0.0f);
	//--------------------------------------------------------------------------------------
	
	// Camera Frame처리
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

	// 마우스 
	//--------------------------------------------------------------------------------------
	MousePosX = mouseX;
	MousePosY = mouseY;
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
	fogStart = 250.0f;
	fogEnd = 550.0f;
	//-------------------------------------------------------------------------------------

	//기본행렬
	//-------------------------------------------------------------------------------------
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
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
	m_Shader->RenderSkydomeShader( m_Sky->GetIndexCount(), SkyworldMatrix, viewMatrix, projectionMatrix,
		m_Sky->GetApexColor(), m_Sky->GetCenterColor());

	//컬링모드켜기
	Rasterizer::Get()->SetOnCullMode();

	//알파블렌딩 혼합 상태 지정
	Blender::Get()->SetAdd();

	m_Sky->RenderCloud();

	m_Shader->RenderCloudShader( m_Sky->GetCloudIndexCount(), SkyworldMatrix, viewMatrix, projectionMatrix,
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

	// Model들
	//-------------------------------------------------------------------------------------
	m_horse->Render();

	m_cube->Render();
	m_Shader->RenderTextureShader(m_cube->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_cube->GetTexture());
	//-------------------------------------------------------------------------------------

	// 2D이미지
	//-------------------------------------------------------------------------------------
	m_MouseCursor->Render(MousePosX, MousePosY);
	
	//-------------------------------------------------------------------------------------



	//버퍼에 그려진 씬을 화면에 표시한다.
	D3D::Get()->EndScene();
	



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